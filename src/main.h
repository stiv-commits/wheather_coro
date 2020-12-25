#pragma once

#include "logger.h"
#include "version.h"


inline void LogStart()
{
  LOG4CPLUS_INFO(getLogger(), "===================================================================================");
  LOG4CPLUS_INFO(getLogger(), "> Starting MSHD Release " << VERSION_INFO);
  LOG4CPLUS_INFO(getLogger(), "===================================================================================");

}

inline void VersionInfo()
{
  LOG4CPLUS_INFO(getLogger(), "===================================================================================");
  LOG4CPLUS_INFO(getLogger(), "> MSHD Release " << VERSION_INFO);
  LOG4CPLUS_INFO(getLogger(), "===================================================================================");

}