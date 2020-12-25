#pragma once

#include "MdrCommonCfg.h"
#include "timertaskcfg.h"
#include "statistic.h"
#include "DbCfg.h"
#include "StrmCfg.h"
#include "ShdConfig.h"

namespace mshd
{
  struct Config
  {
    struct General
    {
      std::string logCfgPath;
      std::string cfgPath;
      std::string tempPath;
    }general;

    mdr::Config mdrCfg;
    TimerTaskCfg timertaskCfg;
    StatSettings statCfg;
    db::Config dbConfig;
    strm::Config strmConfig;
    ShdConfiguration shdConfig;
  };
}