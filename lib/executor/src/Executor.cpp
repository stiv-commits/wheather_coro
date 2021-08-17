#include "Executor.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

namespace ex
{
  struct Executor::Idle
  {
    Idle(boost::asio::io_context& ioc)
      :idle(std::make_unique<boost::asio::io_service::work>(ioc))
    {}
    ~Idle() {}
    std::unique_ptr<boost::asio::io_service::work> idle;
  };

  Executor::Executor(const Config& cfg)
    :ioc_(std::make_shared<boost::asio::io_context>()), cfg_(cfg), idle_(std::make_unique <Executor::Idle>(*ioc_))
  {
  }

  Executor::~Executor()
  {}

  void Executor::Run()
  {
    for (uint32_t i = 0; i < cfg_.num_cpu; i++)
      ioctxThreads_.emplace_back([&, i]
    {
      LOG_INFO("WorkThread: thread started (PID ')'");

      try
      {
        boost::system::error_code ec;
        ioc_->run(ec);
        if (ec) LOG_ERROR("Exception ioc:run: " << ec << " - " << ec.message());
      }
      catch (const std::exception& e)
      {
        LOG_ERROR("Exception ioc:run: " << e.what());
      }
    });
  }

  void Executor::Stop()
  {
    idle_.reset();

    for (auto& it : ioctxThreads_)
    {
      if (it.joinable()) it.join();
    }
  }
}