#pragma once

#include <deque>
#include <thread>
#include <memory>
#include "cfg.h"
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>


namespace httpclient
{
  class HttpClient;
  using HttpClientPtr = std::shared_ptr<HttpClient>;

  class ManagerConn
  {
  public:
    struct Dependencies
    {
      Dependencies(boost::asio::io_context& ioc_)
        :ioc(ioc_)
      {}

      boost::asio::io_context& ioc;
    };

    struct Config
    {
      uint8_t numConn = 1;
      std::string url;
      std::string port;
      std::string nameClient;
    };

    ManagerConn(const Config& cfg, const Dependencies& dep);
    void Init();
    boost::asio::awaitable<boost::beast::http::response<boost::beast::http::dynamic_body>>
      SendRecvAsync(const boost::beast::http::request<boost::beast::http::string_body>& req);
    void Stop();

  private:

    boost::asio::awaitable<HttpClientPtr> GetClient();
    boost::asio::awaitable<void> Reconnect(HttpClientPtr httpClient);
    boost::asio::awaitable<void> Connect(HttpClientPtr httpClientArg);
    boost::asio::awaitable<void> Disconnect(HttpClientPtr httpClient);

    Config cfg_;
    Dependencies dep_;

    std::mutex mut_;
    std::deque<HttpClientPtr> httpClients_;
    std::atomic_bool fEnd_ = false;
    std::string prefixLogConn_;
  };
  using ManagerConnPtr = std::shared_ptr<ManagerConn>;
}