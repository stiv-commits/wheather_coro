#include <string>
#include <iostream>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/winapi/dll.hpp>

#include "WeatherManager.h"
#include "IniLoader.h"
#include "main.h"
const int retError_ = -1;
const int retSuccess_ = 0;

#include "logger.h"


//extern void HttpStart();

int main(int argc, char** argv)
{
  std::string inifile;
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
    VersionInfo();
    return retSuccess_;
  }
  
  LogStart();

  //setlocale(LC_ALL, "Russian_Russia.1251");
  //setlocale(LC_ALL, "ru_RU.UTF-8");

  IniLoader iniLoader;
  if (!iniLoader.LoadConfiguration(inifile)) return 1;
  
  {
    Manager manager(iniLoader.GetSettings());
    manager.Run();

    //HttpStart();
    getchar();
    manager.Stop();
  }

  return retSuccess_;
}
