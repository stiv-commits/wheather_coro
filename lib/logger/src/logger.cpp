#include <logger.h>

#include <iostream>
#include <thread>
#include <boost/date_time.hpp>
#include <string_view>
#include <fstream>
#include <mutex>

static logger::LogLevel level_ = logger::TRACE;
static std::ofstream logFileStream;
static bool fOutToFile = false;
static std::mutex m_;

void logger::LogEvent(const std::string_view& data, LogLevel l)
{
  std::lock_guard lock(m_);
  if (!fOutToFile) std::cout << data << std::endl;
  else logFileStream << data << std::endl;
}

void logger::SetLogFile(const char * log_file)
{
  logFileStream.open(log_file, std::ios::out | std::ios::app);
  if (!logFileStream.is_open())
  {
    std::cout << "Failed to open log file: " << log_file << '\n';
  }
  else fOutToFile = true;
}

void logger::SetLogLevel(logger::LogLevel level)
{
  level_ = level;
}

logger::LogLevel logger::GetLogLevel()
{
  return level_;
}

std::string logger::LevelToStr(logger::LogLevel l)
{
  switch (l) {
  case NONE: return "None";
  case ERRORLOG: return "ERROR";
  case WARN: return "WARN";
  case INFO: return "INFO";
  case DEBUG: return "DEBUG";
  case TRACE: return "TRACE";
  }
  return "NONE";
}

inline char const* GetFileName(char const * file)
{
  char sep = '/';

#ifdef _WIN32
  sep = '\\';
#endif

  auto pLastSlash = strrchr(file, sep);
  return pLastSlash ? pLastSlash + 1 : file;
}


void logger::FormatPrefix(std::stringstream& ss, logger::LogLevel l, char const * file, int line)
{
  std::time_t t = std::time(nullptr);
  ss << std::put_time(std::localtime(&t), "%F %T");
  ss << " [" << GetFileName(file) << ":" << line << "]";
  ss << " [" << std::this_thread::get_id() << "]";
  ss << " [" << LevelToStr((l)) << "] ";
}


void logger::FormatPrefix(logger::LogHelper& ss, LogLevel l, char const * file, int line)
{
  std::time_t t = std::time(nullptr);
  ss << std::put_time(std::localtime(&t), "%F %T");
  //ss << boost::posix_time::second_clock::local_time(); // very very slow
  ss << " [" << GetFileName(file) << ":" << line << "]";
  ss << " [" << std::this_thread::get_id() << "]";
  ss << " [" << LevelToStr((l)) << "] ";
}