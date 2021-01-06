#pragma once

#include "WConfig.h"


class IniLoader
{
public:
  IniLoader() = default;
  ~IniLoader() = default;

  IniSettings GetSettings() const;
  bool LoadConfiguration(const std::string& fname);

private:

  IniSettings cfg_;
};
