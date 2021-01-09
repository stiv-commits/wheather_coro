#pragma once

#include "WConfig.h"
#include "Server.h"

#include "Executor.h"
#include "YanCoor.h"
#include "WeatherClient.h"
#include "DbIntf.h"

#include <memory>
#include <set>

//class HttpSession;
//using HttpSessionPtr = std::shared_ptr<HttpSession>;
//
//class YanCoorClient;
//using YanCoorClientPtr = std::shared_ptr<YanCoorClient>;
//
//class WeatherClient;
//using WeatherClientPtr = std::shared_ptr<WeatherClient>;

class Manager
{
public:
  Manager(const IniSettings& cfg);
  ~Manager() = default;
  void Run();
  void Stop();

private:

  //void getcoor(const Address& adr, boost::asio::yield_context yld);

  IniSettings cfg_;
  ex::ExecutorPtr executor_;

  ServerPtr srv_;
  //std::set<HttpSessionPtr> sessions_;
  YanCoorClientPtr geoClient_;
  WeatherClientPtr weatherClient_;
  DbIntfPtr dbIntf_;
};
