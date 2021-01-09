
#include "WeatherManager.h"
#include "HttpSession.h"
#include "InMemoryDb.h"
#include "Postgre.h"
#include "logger.h"

#include <thread>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace bl
{

  struct Dependencies
  {
    Dependencies(boost::asio::io_context& ioc_)
      :ioc(ioc_)
    {}

    boost::asio::io_context& ioc;
    HttpSessionPtr httpSession;
    YanCoorClientPtr geoClient;
    WeatherClientPtr weatherClient;
    DbIntfPtr db;
  };

  boost::asio::awaitable<void> BusinessLogic(Dependencies dep, CommonCfg cfg)
  {
    auto adr = co_await dep.httpSession->GetRequest();
    if (adr.Empty())
    {
      co_await dep.httpSession->SendAnswerError("Cant get request");
      co_return;
    }

    auto coor = dep.db->GetCoorByAdrress(adr, cfg.livetime);
    if (coor.Empty())
    {
      coor = co_await dep.geoClient->GetCoordinate(adr);
      if (coor.Empty())
      {
        co_await dep.httpSession->SendAnswerError("Cant get coordinate");
        co_return;
      }
      dep.db->PutCoordinate(adr, coor);
    }

    auto weather = dep.db->GetWeatherByCoor(coor, cfg.livetime, cfg.distance);
    if (weather.Empty())
    {
      weather = co_await dep.weatherClient->GetWeather(coor);
      if (weather.Empty())
      {
        co_await dep.httpSession->SendAnswerError("Cant get weather");
        co_return;
      }
      dep.db->PutWeather(coor, weather);
    }

    co_await dep.httpSession->SendAnswer(weather);

    boost::asio::deadline_timer timer(dep.ioc);
    timer.expires_from_now(boost::posix_time::seconds(cfg.requestPause));
    co_await timer.async_wait(boost::asio::use_awaitable);
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

      bl::Dependencies depbl(executor_->GetExecutor());
      depbl.httpSession = session;
      depbl.geoClient = geoClient_;
      depbl.weatherClient = weatherClient_;
      depbl.db = dbIntf_;
      boost::asio::co_spawn(executor_->GetExecutor(), bl::BusinessLogic(depbl, cfg_.commonCfg), boost::asio::detached);
    });

  YanCoorClient::Dependencies depy{ executor_->GetExecutor() };
  geoClient_ = std::make_shared<YanCoorClient>(cfg_.geoCfg, depy);

  WeatherClient::Dependencies depw{ executor_->GetExecutor() };
  weatherClient_ = std::make_shared<WeatherClient>(cfg_.weathCfg, depw);

  if (cfg_.commonCfg.typeStorage == "in_memory") dbIntf_ = std::make_shared<InMemoryDb>(InMemoryDb::Config{}, InMemoryDb::Dependencies{});
  else if (cfg_.commonCfg.typeStorage == "postgre") dbIntf_ = std::make_shared<Postgre>(Postgre::Config{}, Postgre::Dependencies{});
  else LOG_ERROR("Unknow type storage : " << cfg_.commonCfg.typeStorage);
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
