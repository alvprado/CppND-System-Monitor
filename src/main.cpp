#include <iostream>

#include "linux_parser.h"
#include "ncurses_display.h"
#include "system.h"

int main() {
  // System system;
  // NCursesDisplay::Display(system);

  float memory = LinuxParser::MemoryUtilization();
  std::cout << "Memory Utilization: " << memory << std::endl;

  long uptime = LinuxParser::UpTime();
  std::cout << "Uptime: " << uptime << " s" << std::endl;

  long active_jiffies_pid = LinuxParser::ActiveJiffies(4131);
  std::cout << "Active jiffies of 4131 pid: " << active_jiffies_pid
            << std::endl;

  long active_jiffies = LinuxParser::ActiveJiffies();
  std::cout << "Active jiffies: " << active_jiffies << std::endl;

  long idle_jiffies = LinuxParser::IdleJiffies();
  std::cout << "Idle jiffies: " << idle_jiffies << std::endl;

  int processes = LinuxParser::TotalProcesses();
  std::cout << "Total processes: " << processes << std::endl;

  int running_processes = LinuxParser::RunningProcesses();
  std::cout << "Running processes: " << running_processes << std::endl;

  std::string cmd_line = LinuxParser::Command(4131);
  std::cout << "Command line of 4131 pid: " << cmd_line << std::endl;

  std::string ram = LinuxParser::Ram(4131);
  std::cout << "RAM of 4131 pid: " << ram << std::endl;

  std::string uid = LinuxParser::Uid(4131);
  std::cout << "Uid of 4131 pid: " << uid << std::endl;

  std::string username = LinuxParser::User(4131);
  std::cout << "User of 4131 pid: " << username << std::endl;

  long uptime_pid = LinuxParser::UpTime(4131);
  std::cout << "Uptime of 4131 pid: " << uptime_pid << " s" << std::endl;
}