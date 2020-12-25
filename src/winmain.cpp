#include <string>
#include <iostream>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/winapi/dll.hpp>

#include "version.h"
#include "MshdManager.h"
#include "IniLoader.h"
#include "main.h"
const int retError_ = -1;
const int retSuccess_ = 0;

std::string replaceFileExt(std::string fname, const std::string& ext)
{
  if (fname.empty())
  {
    char path[_MAX_PATH + 1];
    if (GetModuleFileNameA(0, path, sizeof(path) / sizeof(path[0])) > 0)
    {
      fname = path;
    }
  }

  if (fname.rfind(".") != std::string::npos)
  {
    fname.replace(fname.rfind("."), ext.length(), ext);
  }
  else
  {
    fname.append(ext);
  }

  return fname;
}
#include "logger.h"
void InitLogger()
{
  log4cplus::BasicConfigurator config;
  config.configure();
  log4cplus::Logger logger = log4cplus::Logger::getInstance(
    LOG4CPLUS_TEXT("main"));
  logger.setLogLevel(log4cplus::ALL_LOG_LEVEL);
  initializeLogger(logger);
}


int main(int argc, char** argv)
{
  std::string inifile;
  //INILoader iniLoader;

  using boost::program_options::value;
  using boost::program_options::variables_map;
  using boost::program_options::command_line_parser;

  variables_map vm;
  boost::program_options::options_description desc("Console options");

  desc.add_options()
    ("help,h", "show help messages")
    ("version", "show version")
    ("inifile", value<std::string>(&inifile), "ini file");

  try
  {
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);
  }
  catch (const std::exception& error)
  {
    std::cout << "Fault to parse command line: " << error.what() << std::endl;
    return retError_;
  }

  if (vm.count("help") != 0U)
  {
    std::cout << "Please use with next argument:" << std::endl;
    std::cout << "--inifile=[configuration file]" << std::endl;
    return retSuccess_;
  }

  if (vm.count("version") != 0U)
  {
    std::cout << GetVersionString() << std::endl;
    return retSuccess_;
  }

  if (inifile.empty())
  {
    inifile = replaceFileExt(argv[0], ".ini");
  }


  //setlocale(LC_ALL, "Russian_Russia.1251");
  //setlocale(LC_ALL, "ru_RU.UTF-8");

  mshd::Config cfg;
  IniLoader iniLoader;
  if (!iniLoader.LoadConfiguration(inifile)) return 1;

  if (!initializeLogger(iniLoader.GetSettings().general.logCfgPath.c_str()/*"D:/work/MSHD/mshd_server/cfg/log.cfg"*/, "filelogger"))
  {
    std::cout << std::endl << "Fault to initialize loggers" << std::endl;
    return false;
  }
  
  LogStart();
  iniLoader.PrintCfg();

  MshdManager mshd(iniLoader.GetSettings());
  mshd.Init();

  mshd.Run();
  getchar();
  mshd.Stop();


  return retSuccess_;
}
