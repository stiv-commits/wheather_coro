#include "IniLoader.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace
{
  void ProcessServerSetings(const boost::property_tree::ptree& pt, IniSettings& cfg_)
  {
    cfg_.serverCfg.ip = pt.get<std::string>("server.ip", "0.0.0.0");
    cfg_.serverCfg.port = pt.get<uint16_t>("server.port", 5000);
  }

  void ProcessCommonSetings(const boost::property_tree::ptree& pt, IniSettings& cfg_)
  {
    cfg_.commonCfg.threads = pt.get<uint16_t>("common.threads", 1);
    cfg_.commonCfg.requestPause = pt.get<uint16_t>("common.request_pause", 1);
    if (cfg_.commonCfg.requestPause > 60) cfg_.commonCfg.requestPause = 60;
    cfg_.commonCfg.typeStorage = pt.get<std::string>("common.type_storage", "in_memory");
    cfg_.commonCfg.livetime = pt.get<uint32_t >("common.live_time", 60);
    cfg_.commonCfg.distance = pt.get<uint32_t >("common.radius", 100);
  }

  void ProcessGeoSetings(const boost::property_tree::ptree& pt, IniSettings& cfg_)
  {
    cfg_.geoCfg.key = pt.get<std::string>("yan_geo.key", "");
    cfg_.geoCfg.url = pt.get<std::string>("yan_geo.url", "");
    cfg_.geoCfg.port = pt.get<std::string>("yan_geo.port", "433");
    cfg_.geoCfg.numConnection = pt.get<uint32_t>("yan_geo.connection", 1);
  }

  void ProcessWeatherSetings(const boost::property_tree::ptree& pt, IniSettings& cfg_)
  {
    cfg_.weathCfg.key = pt.get<std::string>("weather.key", "");
    cfg_.weathCfg.url = pt.get<std::string>("weather.url", "");
    cfg_.weathCfg.port = pt.get<std::string>("weather.port", "433");
    cfg_.weathCfg.numConnection = pt.get<uint32_t>("weather.connection", 1);
  }
}

bool IniLoader::LoadConfiguration(const std::string& fname)
{
  boost::property_tree::ptree pt;

  try
  {
    read_ini(fname, pt);
    ProcessServerSetings(pt, cfg_);
    ProcessCommonSetings(pt, cfg_);
    ProcessGeoSetings(pt, cfg_);
    ProcessWeatherSetings(pt, cfg_);
  }
  catch (const boost::property_tree::ptree_error& error)
  {
    std::cout << "Fault to read ini file: " << error.what() << std::endl;
    return false;
  }

  return true;
}



IniSettings IniLoader::GetSettings() const
{
  return cfg_;
}
