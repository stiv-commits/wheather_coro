#pragma once

#include "WConfig.h"
#include "Server.h"

#include "Executor.h"
#include "YanCoor.h"
#include "WeatherClient.h"
#include "DbIntf.h"

#include <memory>
#include <set>

class Manager
{
public:
  Manager(const IniSettings& cfg);
  ~Manager() = default;
  void Run();
  void Stop();

private:

  IniSettings cfg_;
  ex::ExecutorPtr executor_;

  ServerPtr srv_;
  YanCoorClientPtr geoClient_;
  WeatherClientPtr weatherClient_;
  DbIntfPtr dbIntf_;
};
