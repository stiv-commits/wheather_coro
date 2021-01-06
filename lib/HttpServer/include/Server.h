#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/co_spawn.hpp>
#include <functional>

class Server
{
public:
  struct Config
  {
    std::string ip;
    uint16_t port;
  };

  using callback = std::function<void (boost::asio::ip::tcp::socket&&, uint32_t)>;

  Server(const Config& cfg, boost::asio::io_context& ioc);
  ~Server() = default;

  void Run();
  void Stop();
  void SetCBNewSession(const callback& cb)
  {
    cb_ = cb;
  }

private:
  boost::asio::awaitable<void> DoListen();

  callback cb_;
  Config cfg_;
  boost::asio::io_context& ioc_;
  boost::asio::ip::tcp::acceptor acceptor_;
  uint32_t numConn_ = 0;
};
using ServerPtr = std::unique_ptr<Server>;
