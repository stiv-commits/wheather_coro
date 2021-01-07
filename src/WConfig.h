#pragma once

#include <string>
#include <cstdlib>
#include <functional>
#include <string>

#include "Server.h"
#include "YanCoor.h"
#include "WeatherClient.h"

struct CommonCfg
{
  uint16_t threads = 0;
  uint16_t requestPause = 0;
};

struct IniSettings
{
  Server::Config serverCfg;
  YanCoorClient::Config geoCfg;
  CommonCfg commonCfg;
  WeatherClient::Config weathCfg;
};


