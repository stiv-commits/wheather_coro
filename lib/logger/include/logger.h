
#pragma once

#include <ostream>
#include <array>
#include <optional>
#include <charconv>
#include <cstring>


#define LOG_EVENT_2(level, logEvent) \
    if (logger::GetLogLevel() >= level){ \
    logger::LogHelper ss; \
    FormatPrefix(ss, level, __FILE__, __LINE__); \
    ss << logEvent; \
    logger::LogEvent(std::string_view(ss.bufStr.data(), ss.len), level);\
  }

#define LOG_ERROR(logEvent) LOG_EVENT_2(logger::LogLevel::ERRORLOG, logEvent)
#define LOG_WARN(logEvent) LOG_EVENT_2(logger::LogLevel::WARN, logEvent)
#define LOG_INFO(logEvent) LOG_EVENT_2(logger::LogLevel::INFO, logEvent)
#define LOG_DEBUG(logEvent) LOG_EVENT_2(logger::LogLevel::DEBUG, logEvent)
#define LOG_TRACE(logEvent) LOG_EVENT_2(logger::LogLevel::TRACE, logEvent)


namespace logger
{

  enum LogLevel {
    NONE,
    ERRORLOG,
    WARN,
    INFO,
    DEBUG,
    TRACE,
  };
  std::string LevelToStr(LogLevel l);
  void FormatPrefix(std::stringstream& ss, LogLevel l, char const *, int);
  void SetLogFile(const char * log_file);

  using TLogInterface = void(const char*, const int);

  void LogEvent(const std::string_view& data, LogLevel l);
  LogLevel GetLogLevel();
  void SetLogLevel(LogLevel);

  struct LogHelper
  {
    std::array<char, 512> bufStr;
    size_t len = 0;

    struct view_strm final : public std::streambuf
    {
      view_strm(char* ptr, size_t len)
      {
        std::streambuf::setp(ptr, ptr + len);
      }
      void set(char* ptr, size_t len)
      {
        std::streambuf::setp(ptr, ptr + len);
      }

      inline size_t GetWritedLen(const char* ptr)
      {
        return std::streambuf::pptr() - ptr;
      }
    };
    std::optional<view_strm> ostrm;

    template <typename T>
    LogHelper& operator<<(const T& val_)
    {
      if constexpr (std::is_constructible<std::string_view, T>::value)
      {
        std::string_view val(val_);
        if (bufStr.max_size() - len <= val.size()) return *this;
        std::memcpy(&bufStr[len], val.data(), val.size());
        len += val.size();
      }
      else if constexpr (std::is_arithmetic<T>::value)
      {
        char Buffer[32];
        const auto Result = std::to_chars(&Buffer[0], &Buffer[32], val_);
        auto size = static_cast<size_t>(Result.ptr - &Buffer[0]);
        if (bufStr.max_size() - len <= size) return *this;
        std::memcpy(&bufStr[len], Buffer, size);
        len += size;
      }
      else 
      {
        if (!ostrm) ostrm.emplace(view_strm(bufStr.data() + len, bufStr.max_size() - len));
        else ostrm->set(bufStr.data() + len, bufStr.max_size() - len);

        std::ostream out(&*ostrm);
        out << val_;

        len += ostrm->GetWritedLen(bufStr.data() + len);
      }
      return *this;
    }

  };
  void FormatPrefix(logger::LogHelper& ss, LogLevel l, char const *, int);

}
