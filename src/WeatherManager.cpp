
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
  };

  boost::asio::awaitable<void> BusinessLogic(Dependencies dep)
  {
    auto adr = co_await dep.httpSession->GetRequest();
    if (adr.Empty()) co_return;


  }
}

Manager::Manager(const IniSettings& cfg)
  :cfg_(cfg)
{
  ex::Executor::Config cfgEx;
  cfgEx.num_cpu = cfg_.commonCfg.threads;
  executor_ = std::make_shared<ex::Executor>(cfgEx);

  Server::Config cfgsrv;
  cfgsrv.ip = cfg_.serverCfg.ip;
  cfgsrv.port = cfg_.serverCfg.port;
  srv_ = std::make_unique<Server>(cfgsrv, executor_->GetExecutor());

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
      boost::asio::co_spawn(executor_->GetExecutor(), bl::BusinessLogic(depbl), boost::asio::detached);


    });
}


//void Manager::getcoor(const Address& adr, boost::asio::yield_context yld)
//{
//  auto coor = geoClient_->get_async(yld, adr);
//}

void Manager::Stop()
{
  srv_->Stop();
  executor_->Stop();
}
void Manager::Run()
{

  srv_->Run();
  executor_->Run();

}
