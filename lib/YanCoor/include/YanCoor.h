#pragma once

#include <memory>

#include "common.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>

namespace httpclient
{
  class ManagerConn;
  using ManagerConnPtr = std::shared_ptr<ManagerConn>;
}

class YanCoorClient 
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

  YanCoorClient(const Config& cfg, const Dependencies& dep);
  ~YanCoorClient();

  bool Init();
  void Stop();
  boost::asio::awaitable<Coordinate> GetCoordinate(const Address& adr);
  
private:

  Config cfg_;
  Dependencies dep_;
  std::string prefixLogConn_;
  httpclient::ManagerConnPtr httpsClient_;
};
using YanCoorClientPtr = std::shared_ptr<YanCoorClient>;