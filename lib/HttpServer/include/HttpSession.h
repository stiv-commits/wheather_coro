#pragma once

#include "common.h"
#include <boost/asio/ip/tcp.hpp>
#include "co_spawn_.h"
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/optional.hpp>


class HttpSession;
using HttpSessionPtr = std::shared_ptr<HttpSession>;

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
  using StopCallBack = std::function<void(HttpSessionPtr)>;

  struct Dependecies
  {
    boost::asio::io_context& ioc;
    boost::asio::ip::tcp::socket socket;
  };
  struct Config
  {
    uint32_t numSession = 0;
  };

  struct ReqParam
  {
    unsigned version = 11;
    bool keepAlive = true;
  };

  HttpSession(Dependecies&& dep, const Config& cfg);
  ~HttpSession();

  boost::asio::awaitable<Address> GetRequest();
  boost::asio::awaitable<void> SendAnswer(const Weather& weather);
  boost::asio::awaitable<void> SendAnswerError(const std::string& msg);

private:



  Dependecies dep_;
  Config cfg_;
  boost::beast::tcp_stream stream_;
  std::string logPrefix_;
  ReqParam reqParam_;
};
using HttpSessionPtr = std::shared_ptr<HttpSession>;
