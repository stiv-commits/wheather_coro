#include "IniLoader.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

#include <cctype>
#include "version.h"

namespace
{
  void setFullPath(const std::string& path, std::string& value)
  {
    if (path.empty() || value.empty()) return;
    if (value.find("/") != std::string::npos) return;

    std::string slash;
    if (path.back() != '/') slash = "/";

    value = path + slash + value;
  }

  size_t ParseSize(const std::string& strSize)
  {
    if (strSize.empty()) return 0;

    auto byte = *strSize.rbegin();
    byte = std::tolower(byte);
    if (byte == 'b') // last 2 symbol is GB, MB, KB
    {
      std::string sizeDifin(strSize.data() + strSize.size() - 2, 2);
      size_t size = atoi(std::string(strSize.data(), strSize.size() - 2).c_str());
      std::transform(sizeDifin.begin(), sizeDifin.end(), sizeDifin.begin(), [](unsigned char c) { return std::tolower(c); });
      if (sizeDifin == "gb")
      {
        size *= (1024 * 1024 * 1024);
      }
      else if (sizeDifin == "mb")
      {
        size *= (1024 * 1024);
      }
      else if (sizeDifin == "kb")
      {
        size *= (1024);
      }
      else std::cout << "Fault to parse port parameters: " << "Unknow size difenition: " << sizeDifin << " , in parametr: " << strSize << std::endl;
      return size;
    }
    else return atoi(strSize.data());
  }

  std::vector<std::string> ParsePaths(std::string& paths)
  {
    std::vector<std::string > res;
    boost::erase_all(paths, " ");
    boost::split(res, paths, boost::is_any_of(","));

    return res;
  }
}

bool IniLoader::LoadConfiguration(const std::string& fname)
{
  boost::property_tree::ptree pt;

  try
  {
    read_ini(fname, pt);
    GeneralSetings(pt);
    MdrSetings(pt);
    StatSetings(pt);
    JimmySetings(pt);
    DbSetings(pt);
    CpuCfg(pt);
    StreamWriter(pt);
    Shd(pt);
  }
  catch (const boost::property_tree::ptree_error& error)
  {
    std::cout << "Fault to read ini file: " << error.what() << std::endl;
    return false;
  }

  setPath();

  return true;
}

void IniLoader::MdrSetings(const boost::property_tree::ptree& pt)
{
  cfg_.mdrCfg.ip = pt.get<std::string>("server.ip", "0.0.0.0");
  cfg_.mdrCfg.port = pt.get<uint16_t>("server.port", 5002);
}

void IniLoader::StatSetings(const boost::property_tree::ptree& pt)
{
  cfg_.statCfg.productVersion = MSHD_PRODUCT_BUILD_VERSION;
  cfg_.statCfg.productJenkinsBuild = MSHD_BUILD_NUMBER;
  cfg_.statCfg.productBuildVersion = MSHD_PRODUCT_BUILD_VERSION;
  cfg_.statCfg.snapshotFile = pt.get<std::string>("stat.stat_snapshot_file", "current_stat.txt");
  cfg_.statCfg.snapshotFileForWeb = pt.get<std::string>("stat.stat_snapshot_file_for_web", "current_stat_web.txt");
  cfg_.statCfg.updateInterval = pt.get<int>("stat.stat_interval", 60);
}

void IniLoader::JimmySetings(const boost::property_tree::ptree& pt)
{
  //cfg_.cpuCfg.jimmyThreads.num_cpu = { pt.get<uint16_t>("cpu_usage.jimmy_num", 0) };
}



void IniLoader::GeneralSetings(const boost::property_tree::ptree& pt)
{
  cfg_.general.cfgPath = pt.get<std::string>("general.cfg_path", "");
  cfg_.general.logCfgPath = pt.get<std::string>("general.log_cfg", "log.log4cplus");
  cfg_.general.tempPath = pt.get<std::string>("general.temp_path", "");
}

mshd::Config IniLoader::GetSettings() const
{
  return cfg_;
}

void IniLoader::setPath()
{
  setFullPath(cfg_.general.tempPath, cfg_.statCfg.snapshotFile);
  setFullPath(cfg_.general.cfgPath, cfg_.general.logCfgPath);
}

void IniLoader::PrintCfg()
{

}

void IniLoader::DbSetings(const boost::property_tree::ptree& pt)
{
  auto size = pt.get<std::string>("db.dump_size", "1GB");
  cfg_.dbConfig.DumpSize = ::ParseSize(size);
  cfg_.dbConfig.DumpMinutes = pt.get<uint32_t>("db.dump_minutes", 60);
  cfg_.dbConfig.dbConfigStrWrite = pt.get<std::string>("db.db_config_write", "");
  cfg_.dbConfig.dbConfigStrRead = pt.get<std::string>("db.db_config_read", "");
  cfg_.dbConfig.NumActiveDb = pt.get<size_t>("db.active_num", 1);

  auto paths = pt.get<std::string>("db.paths", "");
  cfg_.dbConfig.paths = ::ParsePaths(paths);

  cfg_.dbConfig.nextDirStrategy = pt.get<std::string>("db.paths_strategy", "");

  cfg_.dbConfig.ConnHashTableSize = pt.get<size_t>("db.conn_hash_t_size", 100);
  cfg_.dbConfig.timeliveDbDays = pt.get<size_t>("db.timelive_db_days", 182);
  cfg_.dbConfig.compress = pt.get<bool>("db.compress", true);
  cfg_.dbConfig.dbConfigStrWal = pt.get<std::string>("db.db_config_wal", "");
}

template<typename T>
void setPriorityPorts(std::vector<T>& ports, const std::string& strprt)
{
  if (strprt.empty()) return;

  std::string temp_str = strprt;

  T port;
  std::vector<std::string> items;

  try
  {
    boost::erase_all(temp_str, " ");
    boost::split(items, temp_str, boost::is_any_of(","));

    for (size_t i = 0; i < items.size(); i++)
    {
      port = boost::lexical_cast<T>(items[i]);

      ports.push_back(port);
    }

    std::sort(ports.begin(), ports.end());
    ports.erase(std::unique(ports.begin(), ports.end()), ports.end());

  }
  catch (...)
  {
    std::cout << "Fault to parse port parameters: " << std::endl;
  }
}


void IniLoader::CpuCfg(const boost::property_tree::ptree& pt)
{
  setPriorityPorts(cfg_.mdrCfg.execCfg.num_cpu, pt.get<std::string>("cpu_usage.srv_num", "-1"));
  setPriorityPorts(cfg_.dbConfig.execCfg.num_cpu, pt.get<std::string>("cpu_usage.db_num", "-1"));
  setPriorityPorts(cfg_.strmConfig.NumCpu, pt.get<std::string>("cpu_usage.writedb", "-1"));
  setPriorityPorts(cfg_.dbConfig.fsync_cpu, pt.get<std::string>("cpu_usage.fsync", "-1"));
}

void IniLoader::StreamWriter(const boost::property_tree::ptree& pt)
{
  cfg_.strmConfig.sizeAllocate = pt.get<int>("stream.pack_alocated", 10000);
  cfg_.strmConfig.queueSize = pt.get<uint32_t>("stream.queue_size", 10000);
}

void IniLoader::Shd(const boost::property_tree::ptree& pt)
{
  cfg_.shdConfig.ConnectionString = pt.get<std::string>("shd.conn_string", "");
  cfg_.shdConfig.DebugLevel = pt.get<uint32_t>("shd.log_level", 0);
  cfg_.shdConfig.LogFile = pt.get<std::string>("shd.log_file", "");
  cfg_.shdConfig.Path = pt.get<std::string>("shd.libpath", "");
  cfg_.shdConfig.PayloadCacheSize = pt.get<uint32_t>("shd.payload_cache", 0);
}