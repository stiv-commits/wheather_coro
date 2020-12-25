#pragma once

#include <thread>
#include <vector>

namespace boost
{
  namespace asio
  {
    class io_context;
  }
}
using io_context_ptr = std::shared_ptr<boost::asio::io_context>;

namespace ex
{
  class Executor
  {
  public:
    struct Config
    {
      uint32_t num_cpu;
    };

    Executor(const Config& cfg);
    ~Executor();
    void Run();
    void Stop();
    inline boost::asio::io_context& GetExecutor()
    {
      return *ioc_;
    }

  private:

    io_context_ptr ioc_;
    Config cfg_;
    std::vector<std::thread> ioctxThreads_;
    struct Idle;
    std::unique_ptr<Idle> idle_;

  };
  using ExecutorPtr = std::shared_ptr<Executor>;
}