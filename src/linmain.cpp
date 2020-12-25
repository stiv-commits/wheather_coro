#include <string>
#include <iostream>
#include <fstream>
#include <signal.h>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
//#include <boost/winapi/dll.hpp>

#include "version.h"
#include "MshdManager.h"
#include "IniLoader.h"
#include "main.h"

#include "logger.h"
const int retError_ = -1;
const int retSuccess_ = 0;
bool isRunning_ = false;
const int rotateInfo_ = 900;

void signalHandler(int sigrcv)
{
  switch (sigrcv)
  {
  case SIGTERM:
    isRunning_ = false;
    break;
  case SIGSTOP:
    isRunning_ = false;
    break;
  case SIGINT:
    isRunning_ = false;
    break;
  case SIGQUIT:
    isRunning_ = false;
    break;
  }
}

void registerSignals()
{
  struct sigaction sa {};
  sigset_t signset;
  sigemptyset(&signset);
  sigaddset(&signset, SIGHUP);
  sigprocmask(SIG_BLOCK, &signset, 0);

  sa.sa_handler = signalHandler;
  sigaction(SIGTERM, &sa, nullptr);
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGQUIT, &sa, nullptr);
  sigaction(SIGSTOP, &sa, nullptr);
  sigaction(SIGUSR1, &sa, nullptr);
  sigaction(SIGUSR2, &sa, nullptr);
}

int startServiceMode(std::string inifileName, std::string pidfileName)
{
  pid_t pid = fork();
  std::string tmpfile = pidfileName;
  //tmpfile.append(".tmp");
  //std::ofstream logfile;;

  if (pid < 0)
  {
    std::cout << "Fault to run child process!" << std::endl;
    return retError_;
  }
  else
  {
    if (pid > 0 && !pidfileName.empty())
    {
      std::ofstream pidfile(pidfileName.c_str(), std::ios::trunc);
      if (!pidfile.is_open())
      {
        std::cout << "Fault to open pid file for write!" << std::endl;
      }
      else
      {
        pidfile << pid << std::endl;
        pidfile.close();
      }
    }
    else
    {
      if (pid == 0)
      {
        int rotate = 0;
        isRunning_ = true;
        registerSignals();
        //logfile.open(tmpfile.c_str(), std::ios::trunc);
        //std::cout.rdbuf(logfile.rdbuf());
        IniLoader iniLoader;

        if (!iniLoader.LoadConfiguration(inifileName))
          return retError_;

        if (!initializeLogger(iniLoader.GetSettings().general.logCfgPath.c_str(), "filelogger"))
        {
          std::cout << std::endl << "Fault to initialize loggers" << std::endl;
          return 1;
        }

        LogStart();
        iniLoader.PrintCfg();

//        if (!iniLoader.GetSettings().runuser.empty())
//        {
//          if (!registerUser(iniLoader.getSettings().runuser))
//          {
//            return false;
//          }
//        }
        {
          MshdManager mshd(iniLoader.GetSettings());

          if (!mshd.Init())
            return retError_;

          mshd.Run();

          while (isRunning_)
          {
            if (++rotate > rotateInfo_)
            {
              try
              {
                rotate = 0;
                //logfile.close();
                //logfile.open(tmpfile.c_str(), std::ios::trunc);
                //std::cout.rdbuf(logfile.rdbuf());
              }
              catch (const std::exception & ex)
              {
                std::cerr << "Exception catched in " << __PRETTY_FUNCTION__ << ":" << ex.what() << std::endl;
              }
            }

            sleep(1);
          }
          mshd.Stop();
        }
        //logfile.close();
        exit(0);
      }
    }
  }

  return retSuccess_;
}

int main(int argc, char** argv)
{
  std::string inifile;
  std::string pidfile;
  //INILoader iniLoader;

  using boost::program_options::value;
  using boost::program_options::variables_map;
  using boost::program_options::command_line_parser;

  variables_map vm;
  boost::program_options::options_description desc("Console options");

  desc.add_options()
    ("help,h", "show help messages")
    ("version", "show version")
    ("inifile", value<std::string>(&inifile), "ini file")
    ("pidfile", value<std::string>(&pidfile), "pid file")
    ("service", "service mode");

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
    std::cout << "--pidfile=[pid file]" << std::endl;
    std::cout << "--service [enable service mode]" << std::endl;
    return retSuccess_;
  }

  if (vm.count("version") != 0U)
  {
    std::cout << GetVersionString() << std::endl;
    return retSuccess_;
  }

  if (inifile.empty())
  {
    inifile = argv[0];
    inifile.append(".ini");
  }

  if (vm.count("service") == 0U)
  {
    isRunning_ = true;
    registerSignals();

    IniLoader iniLoader;
    if (!iniLoader.LoadConfiguration(inifile)) return 1;

    if (!initializeLogger(iniLoader.GetSettings().general.logCfgPath.c_str(), "filelogger"))
    {
      std::cout << std::endl << "Fault to initialize loggers" << std::endl;
      return 1;
    }

    LogStart();
    iniLoader.PrintCfg();

    MshdManager mshd(iniLoader.GetSettings());
    mshd.Init();

    mshd.Run();
    while (isRunning_)
    {
      sleep(1);
    }
    mshd.Stop();
  }
  else
  {
    return startServiceMode(inifile, pidfile);
  }

  return retSuccess_;
}
