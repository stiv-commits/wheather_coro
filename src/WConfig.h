#pragma once

#include <string>
#include <cstdlib>
#include <functional>
#include <string>

struct SrvConfig
{
  std::string ip;
  uint16_t port;
};

struct CommonCfg
{
  uint16_t threads = 0;
  uint16_t requestPause = 0;
};

struct GeoCfg
{
  std::string key;
  std::string url;
  std::string port;
  uint32_t numConnection = 1;
};

struct WeathCfg
{
  std::string key;
  std::string url;
  std::string port;
  uint32_t numConnection = 1;
};
struct IniSettings
{
  SrvConfig serverCfg;
  CommonCfg commonCfg;
  GeoCfg geoCfg;
  WeathCfg weathCfg;
};


