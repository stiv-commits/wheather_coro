#include "Server.h"
#include "logger.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/config.hpp>
#include <boost/asio/detached.hpp>

#include <thread>

namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

Server::Server(const Config& srvCfg, boost::asio::io_context& ioc)
  :cfg_(srvCfg), ioc_(ioc), acceptor_(ioc_)
{

}

void Server::Run()
{
  boost::asio::co_spawn(ioc_, DoListen(), boost::asio::detached);
}

void Server::Stop()
{
  boost::system::error_code ec;
  acceptor_.cancel(ec);
}

boost::asio::awaitable<void> Server::DoListen()
{
  tcp::endpoint endpoint{ net::ip::make_address(cfg_.ip), cfg_.port };

  boost::system::error_code ec;

  acceptor_.open(endpoint.protocol(), ec);
  if (ec)
  {
    LOG_ERROR("Cant open acceptor : " << ec.message());
    co_return;
  }

  // Allow address reuse
  acceptor_.set_option(net::socket_base::reuse_address(true), ec);
  if (ec)
  {
    LOG_ERROR("Cant set_option reuse adress: " << ec.message());
    co_return;
  }

  // Bind to the server address
  acceptor_.bind(endpoint, ec);
  if (ec)
  {
    LOG_ERROR("Cant bind enpoint: " << ec.message());
    co_return;
  }

  // Start listening for connections
  acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if (ec)
  {
    LOG_ERROR("Cant listen endpoint: " << ec.message());
    co_return;
  }

  for(;;)
  {
    try {
      tcp::socket socket(ioc_);
      co_await acceptor_.async_accept(socket, boost::asio::use_awaitable);
      LOG_INFO("New Connection " << socket.remote_endpoint() << " , num: " << numConn_+1);
      cb_(std::move(socket), numConn_++);
    }
    catch (std::exception& e) {
      LOG_ERROR("Accept connection on " << endpoint << " failed. Exception: " << e.what());
      co_return;
    }

  }
}


