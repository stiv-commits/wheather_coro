
#include "WeatherManager.h"
#include "HttpSession.h"

#include <thread>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
//#include "WeatherClient.h"
//#include "YanCoorClient.h"

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace bl
{
  struct Dependencies
  {
    HttpSessionPtr httpSession;
    YanCoorClientPtr geoClient;
    WeatherClientPtr weatherClient;
  };

  boost::asio::awaitable<void> BusinessLogic(Dependencies dep)
  {
    auto adr = co_await dep.httpSession->GetRequest();
    if (adr.Empty())
    {
      co_await dep.httpSession->SendAnswerError("Cant get request");
      co_return;
    }

    auto coor = co_await dep.geoClient->GetCoordinate(adr);
    if (coor.Empty())
    {
      co_await dep.httpSession->SendAnswerError("Cant get coordinate");
      co_return;
    }

    auto weather = co_await dep.weatherClient->GetWeather(coor);
    if (weather.Empty())
    {
      co_await dep.httpSession->SendAnswerError("Cant get weather");
      co_return;
    }

    co_await dep.httpSession->SendAnswer(weather);
  }
}

Manager::Manager(const IniSettings& cfg)
  :cfg_(cfg)
{
  ex::Executor::Config cfgEx;
  cfgEx.num_cpu = cfg_.commonCfg.threads;
  executor_ = std::make_shared<ex::Executor>(cfgEx);

  srv_ = std::make_unique<Server>(cfg_.serverCfg, executor_->GetExecutor());

  srv_->SetCBNewSession([&](boost::asio::ip::tcp::socket&& socket, uint32_t connId)
    {

      HttpSession::Dependecies dep{ executor_->GetExecutor(), std::move(socket) };
      HttpSession::Config cfg
      {
        .numSession = connId
      };
      auto session = std::make_shared<HttpSession>(std::move(dep), cfg);

      bl::Dependencies depbl;
      depbl.httpSession = session;
      depbl.geoClient = geoClient_;
      depbl.weatherClient = weatherClient_;
      boost::asio::co_spawn(executor_->GetExecutor(), bl::BusinessLogic(depbl), boost::asio::detached);
    });

  YanCoorClient::Dependencies depy{ executor_->GetExecutor() };
  geoClient_ = std::make_shared<YanCoorClient>(cfg_.geoCfg, depy);

  WeatherClient::Dependencies depw{ executor_->GetExecutor() };
  weatherClient_ = std::make_shared<WeatherClient>(cfg_.weathCfg, depw);
}


void Manager::Stop()
{
  srv_->Stop();
  geoClient_->Stop();
  weatherClient_->Stop();

  executor_->Stop();
}
void Manager::Run()
{
  srv_->Run();
  geoClient_->Init();
  weatherClient_->Init();

  executor_->Run();
}
