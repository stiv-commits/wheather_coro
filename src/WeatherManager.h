#pragma once

#include <cstdint>

#include "MdrManager.h"
#include "MshdConfig.h"
#include "DbsManager.h"
#include "IODbIntf.h"
#include "StrmMngr.h"
#include "RdrManager.h"
#include "WriteMngr.h"

class MshdManager 
{
public:


  MshdManager(const mshd::Config& cfg);
  ~MshdManager();
  bool Init();
  void Run();
  void Stop();

private:

  mshd::Config cfg_;
  mdr::ManagerPtr mdrManager_;

  class AllocatorStatExecuter;
  std::shared_ptr<AllocatorStatExecuter> allocStat_;

  std::shared_ptr <db::DbsManager> dbsManager_;
  std::shared_ptr <wrt::Manager> writer_;

  std::shared_ptr <strm::Manager> strmMgr_;
  
  std::shared_ptr<rdr::Manager> readerMgr_;
};
