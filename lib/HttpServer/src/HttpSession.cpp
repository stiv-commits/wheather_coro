#include "HttpSession.h"
#include <boost/beast/version.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time.hpp>
#include <random>
#include <thread>

#include "logger.h"

#define PREFIX  "[HttpSession:" << __LINE__ << "] [" << this <<"] [" << std::this_thread::get_id() <<"] "
#define hsinfo linfo <<  PREFIX
#define hserr lerr << PREFIX
#define pdeb ldeb << PREFIX

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace
{

  std::string GetValue(const http::request<boost::beast::http::string_body>& req, std::string key)
  {
    std::string val;
    key += '=';

    auto startAdr = req.target().find(key);
    if (startAdr == beast::string_view::npos)
    {
      LOG_ERROR("[HttpSession:" << __LINE__ << "] Illegal request-target: " << req.target() << ", field \"" << key << "\" not found! ");
      return "";
    }
    startAdr += key.size();

    auto end = req.target().find("&", startAdr);

    val = std::string(&req.target()[startAdr], (end == beast::string_view::npos) ? req.target().length() - startAdr
      : end - startAdr);

    return val;
  }

  boost::beast::http::response<boost::beast::http::string_body> CreateWeatherResponse(const Weather& weather, const HttpSession::ReqParam& reqParam)
  {
    http::string_body::value_type body; // TODO = makeBody(weather);
    auto size = body.size();

    http::response<http::string_body> res{
      std::piecewise_construct,
      std::make_tuple(std::move(body)),
      std::make_tuple(http::status::ok, reqParam.version) };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.content_length(size);
    res.keep_alive(reqParam.keepAlive);

    return res;
  }

  Address GetAdress(const http::request<boost::beast::http::string_body>& locreq)
  {
    if (locreq.method() != http::verb::get)
    {
      LOG_ERROR("Unknown HTTP-method: " << locreq.method_string() << " ");
      return Address();
    }

    Address adr;
    adr.city = GetValue(locreq, "city");
    adr.street = GetValue(locreq, "street");
    adr.house = GetValue(locreq, "house");

    return adr;
  }
}

HttpSession::HttpSession(Dependecies&& dep, const Config& cfg)
  :dep_(std::move(dep)), cfg_(cfg), stream_(std::move(dep_.socket))
{
  logPrefix_ = "[HS_" + std::to_string(cfg_.numSession) + "] ";
  LOG_DEBUG(logPrefix_ << "New session");
}

HttpSession::~HttpSession()
{
  LOG_DEBUG(logPrefix_ << "End session");
}

boost::asio::awaitable<Address> HttpSession::GetRequest()
{
  stream_.expires_after(std::chrono::seconds(60));

  boost::beast::flat_buffer buffer;
  http::request<http::string_body> req;

  try {
    co_await http::async_read(stream_, buffer, req, boost::asio::use_awaitable);
  }
  catch (std::exception& e) {
    LOG_ERROR(logPrefix_ << "Cant get request from client. Exception: " << e.what());
    co_return Address{};
  }

  reqParam_.version = req.version();
  reqParam_.keepAlive = req.keep_alive();

  auto adr = GetAdress(req);

  LOG_INFO(logPrefix_ << "Request: " << adr);
  co_return adr;
}

boost::asio::awaitable<void> HttpSession::SendAnswer(const Weather& weather)
{
  auto res = CreateWeatherResponse(weather, reqParam_);
  LOG_INFO(logPrefix_ << "Send answer");
  try {
    co_await http::async_write(stream_, res, boost::asio::use_awaitable);
  }
  catch (std::exception& e) {
    LOG_ERROR(logPrefix_ << "Cant send response to client. Exception: " << e.what());
  }
}

boost::asio::awaitable<void> HttpSession::SendAnswerError(const std::string& msg)
{
  LOG_INFO(logPrefix_ << "Send answer error: " << msg);

  http::response<http::string_body> res{ http::status::internal_server_error, reqParam_.version };
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, "text/html");
  res.keep_alive(reqParam_.keepAlive);
  res.body() = "An error occurred: '" + std::string(msg) + "'";
  res.prepare_payload();

  boost::system::error_code ec;
  try {
    co_await http::async_write(stream_, res, boost::asio::use_awaitable);
  }
  catch (std::exception& e) {
    LOG_ERROR(logPrefix_ << "Cant send error to client. Exception: " << e.what());
  }
}










