#pragma once

#include <memory>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>

#include "common.h"

namespace httpclient
{
  class ManagerConn;
  using ManagerConnPtr = std::shared_ptr<ManagerConn>;
}

class WeatherClient 
{
public:
  struct Config
  {
    std::string key;
    std::string url;
    std::string port;
    uint32_t numConnection = 1;
  };

  struct Dependencies
  {
    Dependencies(boost::asio::io_context& ioc_)
      :ioc(ioc_)
    {}

    boost::asio::io_context& ioc;
  };

  WeatherClient(const Config& cfg, const Dependencies& dep);
  ~WeatherClient();

  bool Init();
  void Stop();

  boost::asio::awaitable <Weather> GetWeather(const Coordinate& coord);

private:

    Config cfg_;
    Dependencies dep_;
    std::string prefixLogConn_;
    httpclient::ManagerConnPtr httpsClient_;
};
using WeatherClientPtr = std::shared_ptr<WeatherClient>;