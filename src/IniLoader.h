#pragma once

#include "MshdConfig.h"

#include <boost/property_tree/ptree.hpp>


class IniLoader
{
public:
  IniLoader() = default;
  ~IniLoader() = default;

  mshd::Config GetSettings() const;
  bool LoadConfiguration(const std::string& fname);
  void PrintCfg();

private:
  void MdrSetings(const boost::property_tree::ptree& pt);
  void StatSetings(const boost::property_tree::ptree& pt);
  void JimmySetings(const boost::property_tree::ptree& pt);
  void GeneralSetings(const boost::property_tree::ptree& pt);
  void DbSetings(const boost::property_tree::ptree& pt);
  void setPath();
  void CpuCfg(const boost::property_tree::ptree& pt);
  void StreamWriter(const boost::property_tree::ptree& pt);
  void Shd(const boost::property_tree::ptree& pt);

  mshd::Config cfg_;
};
