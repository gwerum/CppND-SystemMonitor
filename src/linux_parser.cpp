#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <limits>
#include <filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

//  Helper method to go to specific line of a file
//  Credits for this implementation go to Stack overflow user 'Xeo': 
//  https://stackoverflow.com/questions/5207550/in-c-is-there-a-way-to-go-to-a-specific-line-in-a-text-file
std::ifstream& LinuxParser::GotoLine(std::ifstream& fileStream, unsigned int lineNumber){
    fileStream.seekg(std::ios::beg);
    for(unsigned int i=0; i < lineNumber - 1; ++i){
        fileStream.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return fileStream;
}

//-----------------------------------------------------------------------------
// System
//-----------------------------------------------------------------------------

// Returns operating system name
string LinuxParser::OperatingSystem() {
  string line, key, value;
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

// Returns kernel version of the system
string LinuxParser::Kernel() {
  string os, version, kernel;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    GotoLine(filestream, 1);
    filestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
// Credits: https://gist.github.com/unix-beard/5887218
vector<int> LinuxParser::Pids() {
  vector<int> pids;
    std::string directory_name;
    auto isDigit = [](const std::string& s) { return std::all_of(s.begin(), s.end(), ::isdigit); };
    for (const auto &directory : std::filesystem::directory_iterator(LinuxParser::kProcDirectory)) {
        directory_name = directory.path().stem().string();
        if (isDigit( directory_name ))
            pids.push_back(stoi(directory_name));
    }
  return pids;
}

// Returns the system memory utilization
float LinuxParser::MemoryUtilization() 
{
  int mem_total, mem_available, mem_used;
  float mem_usage;
  string line, key;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    // Get first line ("MemTotal:        3884328 kB")
    GotoLine(filestream, 1);
    filestream >> key >> mem_total;
    // Get third line ("MemAvailable:    3618432 kB")
    GotoLine(filestream, 3);
    filestream >> key >> mem_available;
  }
  // Compute usage
  mem_used = mem_total - mem_available;
  mem_usage = (float) mem_used / (float) mem_total;
  return mem_usage;
}

// Returns the system uptime
long LinuxParser::UpTime() {
  float uptime;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    // Get first and only line of format: "3569.05 14148.62"
    // (Time in seconds since boot-up, total time used by processes)
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return (long) uptime;
 }

 // Returns CPU times values required for computation CPU utilization of total system
vector<string> LinuxParser::CpuUtilization() {
  vector<string> times;
  string skip_cpu_name, time, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    // Read first line with total CPU times
    //
    //      user    nice  system  idle      iowait irq  softirq  steal  guest  guest_nice
    // cpu  1013149 64008 1239265 680592396 403525 0    26296    0      0      0
    //
    // (will be used to compute CPU utilization according to https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux)
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> skip_cpu_name;
    while(linestream >> time) {
      times.push_back(time);
    };
  };
  return times;
}

//-----------------------------------------------------------------------------
// Processor
//-----------------------------------------------------------------------------

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> jiffies = CpuUtilization();
  long total_jiffies = 0;
  for(string jiffie : jiffies) {
    total_jiffies += std::stoi(jiffie);
  }
  return total_jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  long idle_jiffies = 0;
  long idle = std::stoi(jiffies[3]);
  long iowait = std::stoi(jiffies[4]);
  idle_jiffies = idle + iowait;
  return idle_jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long active_jiffies = 0;
  active_jiffies = Jiffies() - IdleJiffies();
  return active_jiffies;
}

// Reads and returns the number of active jiffies for a PID
// Reads jiffie values from file /proc/[pid]/stat: https://man7.org/linux/man-pages/man5/proc.5.html
// Computes CPU utilization according to: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::ActiveJiffies(int pid) { 
  long active_jiffies = 0;
  string utime, stime, cutime, cstime;
  string skip_values, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid)+ kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line); 
    // Reading line with the following format:
    // id   comm     state                                            utime  stime  cutime  cstime            starttime
    // 1823 (mysqld) S      1 1790 1790 0 -1 1077936192 34164 0 110 0 174810 260004 0       0       20 0 31 0 1867        2827812864 34467 18446744073709551615 1 1 0 0 0 0 552967 4096 26345 0 0 0 -1 0 0 0 107738 0 0 0 0 0 0 0 0 0 0
    for(int i = 1; i < 14; ++i) {
      linestream >> skip_values;
    }
    linestream >> utime >> stime >> cutime >> cstime;
  }
  active_jiffies = std::atol(utime.c_str()) + std::atol(stime.c_str()) + std::atol(cutime.c_str()) + std::atol(cstime.c_str());
  return active_jiffies;
}

//-----------------------------------------------------------------------------
// Process
//-----------------------------------------------------------------------------

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    GotoLine(filestream, 9); // line 9 format: "processes 3273"
    filestream >> key >> value;
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    GotoLine(filestream, 10); // line 10 format: "procs_running 1"
    filestream >> key >> value;
  }
  return value;
 }

// Reads and returns the command associated with a process
string LinuxParser::Command(int pid) 
{
  string command;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    GotoLine(filestream, 1); // line 1 format: "/usr/lib/application/command --type=..."
    filestream >> command;
  }
  return command;
}

// Returns the memory used by a process
int LinuxParser::Ram(int pid) {
  string line, key;
  int ram_usage = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {    // line format: root:x:0:0:root:/root:/bin/bash
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> ram_usage) {
        if (key == "VmSize")
          return ram_usage;
      }
    }
  }
  return ram_usage;
}

// Returns the user ID associated with a process
int LinuxParser::Uid(int pid) {
  string key;
  int uid = -1;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    GotoLine(filestream, 9); // line 9 format: "Uid:	0	0	0	0"
    filestream >> key >> uid;
  }
  return uid;
}

// Returns the user associated with a process PID
string LinuxParser::User(int pid) {
  int uid = Uid(pid);
  string user_name = "Unknown";
  string line, x;
  int user_id;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {    // line format: root:x:0:0:root:/root:/bin/bash
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user_name >> x >> user_id) {
        if (user_id == uid)
          return user_name;
      }
    }
  }
  return user_name;
}

// Returns the uptime of a process (in seconds)
long LinuxParser::UpTime(int pid) { 
  long ticks = 0;
  string line;
  string skip_value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line); 
    for(int i = 1; i < 22; ++i) {
      linestream >> skip_value;
    }
    linestream >> ticks;
  }
  return ticks / sysconf(_SC_CLK_TCK);
}
