#include "MshdManager.h"
#include "StrmMngr.h"
#include "logger.h"
#include "timertaskmgr.h"
#include "pktallocator.h"
#include "pktallocator_mcmp.h"

#include "DbsManager.h"
#include "WriteMngr.h"


class MshdManager::AllocatorStatExecuter : public StatShard {
public:
  typedef std::shared_ptr<AllocatorStatExecuter> Ptr;
  void GetStatistic(StatKvPairs& kvp) override
  {
    const auto measurementKoeff = updateIntervalSec_ * (1024 * 1024) / 8; // bytes -> Mb, interval -> sec
    auto allocate_in_packets_ = PktAllocatorStats::allocate_in_packets_.load(std::memory_order_relaxed);
    auto allocate_out_packets_ = PktAllocatorStats::allocate_out_packets_.load(std::memory_order_relaxed);
    auto allocate_in_dynamic_ = PktAllocatorStats::allocate_in_dynamic_.load(std::memory_order_relaxed);
    auto allocate_out_dynamic_ = PktAllocatorStats::allocate_out_dynamic_.load(std::memory_order_relaxed);

    kvp.Set("Allocator.Delta", (int64_t)allocate_in_packets_ - (int64_t)allocate_out_packets_);
    kvp.Set("Allocator.DynamicDelta", (int64_t)allocate_in_dynamic_ - (int64_t)allocate_out_dynamic_);

    kvp.Set("Allocator.PacketsPs", ((int64_t)allocate_in_packets_ - PktAllocatorStats::last_f_in_packets_) / updateIntervalSec_);
    kvp.Set("Allocator.DynamicPs", ((int64_t)allocate_in_dynamic_- PktAllocatorStats::last_f_in_dynamic) / updateIntervalSec_);

    PktAllocatorStats::last_f_in_packets_ = allocate_in_packets_;
    PktAllocatorStats::last_f_in_dynamic = allocate_in_dynamic_;


    {
      auto allocate_in_packets_ = AllocatorMCMP::PktAllocatorStats::allocate_in_packets_.load(std::memory_order_relaxed);
      auto allocate_out_packets_ = AllocatorMCMP::PktAllocatorStats::allocate_out_packets_.load(std::memory_order_relaxed);
      auto allocate_in_dynamic_ = AllocatorMCMP::PktAllocatorStats::allocate_in_dynamic_.load(std::memory_order_relaxed);
      auto allocate_out_dynamic_ = AllocatorMCMP::PktAllocatorStats::allocate_out_dynamic_.load(std::memory_order_relaxed);

      kvp.Set("AllocatorMcmp.Delta", (int64_t)allocate_in_packets_ - (int64_t)allocate_out_packets_);
      kvp.Set("AllocatorMcmp.DynamicDelta", (int64_t)allocate_in_dynamic_ - (int64_t)allocate_out_dynamic_);

      kvp.Set("AllocatorMcmp.PacketsPs", ((int64_t)allocate_in_packets_ - AllocatorMCMP::PktAllocatorStats::last_f_in_packets_) / updateIntervalSec_);
      kvp.Set("AllocatorMcmp.DynamicPs", ((int64_t)allocate_in_dynamic_ - AllocatorMCMP::PktAllocatorStats::last_f_in_dynamic) / updateIntervalSec_);

      AllocatorMCMP::PktAllocatorStats::last_f_in_packets_ = allocate_in_packets_;
      AllocatorMCMP::PktAllocatorStats::last_f_in_dynamic = allocate_in_dynamic_;
    }
  }

  size_t updateIntervalSec_ = 60;
};

MshdManager::MshdManager(const mshd::Config& cfg)
  :cfg_(cfg)
{
  db::Dependencies dep;
  dbsManager_ = std::make_shared<db::DbsManager>(cfg_.dbConfig, dep);

  wrt::Manager::Config cfgActive;
  cfgActive.MaxSizeDump = cfg.dbConfig.DumpSize;
  cfgActive.MaxMinDump = cfg.dbConfig.DumpMinutes;
  cfgActive.PrintStatTime= cfg.statCfg.updateInterval;
  cfgActive.numActive= cfg.dbConfig.NumActiveDb;
  cfgActive.cpuFsync = cfg.dbConfig.fsync_cpu;
  wrt::Manager::Dependencies depActive;
  depActive.dbManager = dbsManager_;
  writer_ = std::make_shared<wrt::Manager>(cfgActive, depActive);

  strm::Dependencies strmDep;
  strmDep.db = writer_;
  strm::Config cfgstrm;
  cfgstrm = cfg_.strmConfig;
  strmMgr_ = std::make_shared<strm::Manager>(cfgstrm, strmDep);

  rdr::Config cfgrdr;
  cfgrdr.PrintStatTime = cfg.statCfg.updateInterval;
  cfgrdr.execCfg = cfg.dbConfig.execCfg;
  cfgrdr.shdCfg = cfg.shdConfig;
  rdr::Dependecies deprdr;
  deprdr.dbManager = dbsManager_;
  readerMgr_ = std::make_shared<rdr::Manager>(cfgrdr, deprdr);

  mdr::Dependencies mdrDep;
  mdrDep.strmMgr = strmMgr_;
  mdrDep.rdrMgr = readerMgr_;
  mdrManager_ = std::make_shared<mdr::Manager>(cfg_.mdrCfg, mdrDep);
}

MshdManager::~MshdManager()
{

}

bool MshdManager::Init()
{
  GetStatSnapshot()->Initialize(cfg_.statCfg);

  if (!getTimerTaskManager()->initialize(cfg_.timertaskCfg))
  {
    LOG4CPLUS_ERROR(getLogger(), "MshdManager::initialize: fault to initialize timertaskmanager");

    return false;
  }
  getTimerTaskManager()->addTask(GetStatSnapshot());

  mdrManager_->Init();
  dbsManager_->Init();

  auto pid = dbsManager_->GetLastStreamId();
  wrt::Manager::InitParam initParam;
  initParam.LastStreamId = pid;
  writer_->Init(initParam);

  readerMgr_->Init();

  strmMgr_->Init();

  allocStat_ = std::make_shared<AllocatorStatExecuter>();
  allocStat_->updateIntervalSec_ = GetStatSnapshot()->GetInterval();
  GetStatSnapshot()->RegisterStatShard(allocStat_.get());

  return true;
}


void MshdManager::Run()
{
  getTimerTaskManager()->run();

  LOG4CPLUS_INFO(getLogger(), "MshdManager::Run()");

  mdrManager_->Run();
  dbsManager_->Run();
  readerMgr_->Run();
  writer_->Run();
}

void MshdManager::Stop()
{
  getTimerTaskManager()->stop();

  mdrManager_->Stop();
  strmMgr_->Stop();
  writer_->Stop();
  dbsManager_->Stop();
  readerMgr_->Stop();
  LOG4CPLUS_INFO(getLogger(), "MshdManager::Stop()");
}
