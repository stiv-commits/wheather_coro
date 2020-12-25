#pragma once

#include <queue>
#include "cfg.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

//class HttpSession;
//using HttpSessionPtr = std::shared_ptr<HttpSession>;



//class HttpsClient
//{
//public:
//  struct HttpsCfg
//  {
//    std::string url;
//    std::string port;
//  };
//
//  HttpsClient(const HttpsCfg& cfg, boost::asio::io_context& ioc);
//
//
//  void Stop();
//
//  boost::beast::http::response<boost::beast::http::dynamic_body> send_recv_async(boost::asio::yield_context& parentYield,
//    boost::beast::http::request<boost::beast::http::string_body> req);
//
//private:
//
//  bool Start(boost::asio::yield_context yld);
//  bool Connect(boost::asio::yield_context& yld);
//  void Disconnect(boost::asio::yield_context& yld);
//  bool OnSendRecvError(const boost::system::error_code& err, boost::asio::yield_context& yld);
//
//  struct ReqAsync
//  {
//    ReqAsync(boost::asio::io_context& ioc)
//      :wakeupTimer(ioc)
//    {}
//    ~ReqAsync() = default;
//
//    boost::asio::deadline_timer wakeupTimer;
//    boost::beast::http::request<boost::beast::http::string_body> req;
//    boost::beast::http::response<boost::beast::http::dynamic_body> res;
//  };
//  using ReqAsyncPtr = std::shared_ptr<ReqAsync>;
//
//  //  callback cb_;
//  HttpsCfg clientCfg_;
//  boost::asio::io_context& ioc_;
//  std::deque<ReqAsyncPtr> data_;
//  std::unique_ptr<boost::asio::deadline_timer> timer_;
//  bool fStop = false;
//  // boost::beast::flat_buffer b;
//  boost::asio::ssl::context ctx_{ boost::asio::ssl::context::sslv23_client };
//  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream_;
//};
