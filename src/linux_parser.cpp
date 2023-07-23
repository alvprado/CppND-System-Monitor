#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using Directory = std::filesystem::directory_entry;
using Path = std::filesystem::path;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS DONE: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  Path proc_path = kProcDirectory;
  Directory proc_dir(proc_path);

  for (const auto& proc_dir_entry :
       std::filesystem::directory_iterator(proc_dir)) {
    if (proc_dir_entry.is_directory()) {
      std::string filename = proc_dir_entry.path().filename().string();
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float total_memory{1.0};
  float free_memory{1.0};
  std::string line;
  std::string key;
  std::string value;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    for (int i = 0; i < 2; i++) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total_memory = std::stof(value);
        } else if (key == "MemFree:") {
          free_memory = std::stof(value);
        }
      }
    }
    stream.close();
  }
  return ((total_memory - free_memory) / total_memory);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  long up_time_s{0};
  std::string line, uptime, idle;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
    stream.close();
  }
  up_time_s = std::stoi(uptime);

  return up_time_s;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  std::string nth_element_of_pid_stat;
  long active_jiffies{0};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 17; i++) {
      linestream >> nth_element_of_pid_stat;
      if (i >= 13) {
        active_jiffies += std::stol(nth_element_of_pid_stat);
      }
    }
    stream.close();
  }
  return active_jiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> cpu_utilization = CpuUtilization();
  long active_jiffies{0};
  for (int i = 0; i < 10; i++) {
    if (i != CPUStates::kIdle_ && i != CPUStates::kIOwait_) {
      active_jiffies += std::stol(cpu_utilization.at(i));
    }
  }
  return active_jiffies;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> cpu_utilization = CpuUtilization();
  return std::stol(cpu_utilization.at(CPUStates::kIdle_)) +
         std::stol(cpu_utilization.at(CPUStates::kIOwait_));
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::vector<std::string> cpu_utilization;
  std::string line, cpu_name, cpu_process_time;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu_name;
    while (linestream >> cpu_process_time) {
      cpu_utilization.push_back(cpu_process_time);
    }
    stream.close();
  }
  return cpu_utilization;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line, key, value;
  int processes{0};

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        processes = std::stoi(value);
        stream.close();
        break;
      }
    }
  }
  return processes;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line, key, value;
  int running_processes{0};

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        running_processes = std::stoi(value);
        stream.close();
        break;
      }
    }
  }
  return running_processes;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string command;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
    stream.close();
  }
  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string line, key, value;
  std::string pid_memory_kB;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        pid_memory_kB = value;
        stream.close();
        break;
      }
    }
  }
  return pid_memory_kB;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string line, key, value;
  std::string uid;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        uid = value;
        stream.close();
        break;
      }
    }
  }
  return uid;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string target_uid = Uid(pid);
  std::string line, user, x, uid;
  std::string username;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if (uid == target_uid) {
        username = user;
        stream.close();
        break;
      }
    }
  }
  return username;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line, nth_element;
  long starttime_jiffies;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> nth_element;
    }
    starttime_jiffies = std::stol(nth_element);
    stream.close();
  }
  return (UpTime() - (starttime_jiffies / sysconf(_SC_CLK_TCK)));
}
