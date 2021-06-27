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

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// TODO: Read and return the system memory utilization
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

// TODO: Read and return the system uptime
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

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<int> LinuxParser::CpuUtilization() {
  vector<int> times;
  int value;
  string cpu, line;
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
    linestream >> cpu;
    while(linestream >> value) {
      times.push_back(value);
    };
  };
  return times;
}

// Returns CPU utilization for PID process
// Reads CPU usage values from file /proc/[pid]/stat: https://man7.org/linux/man-pages/man5/proc.5.html
// Computes CPU utilization according to: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float LinuxParser::CpuUtilization(int pid) {
    std::string line, comm;
    int id, stat;
    char state;
    std::vector<int> stats;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::stringstream linestream(line);
        linestream >> id >> comm >> state;
        // Read residual integer values
        while(linestream >> stat) {
            stats.push_back(stat);
        }
    }
    long clock_frequence = sysconf(_SC_CLK_TCK);
    // Compute total process time including child processes
    int utime, stime, cutime, cstime, starttime, total_time;
    utime = stats[10]; // CPU time spent in user code
    stime = stats[11]; // CPU time spent in kernel code
    cutime = stats[12]; // Waited-for children's CPU time spent in user code
    cstime = stats[13]; // Waited-for children's CPU time spent in kernel code
    starttime = stats[18];  // Time when the process started
    total_time = utime + stime + cutime + cstime;
    float process_time = (float) total_time / (float) clock_frequence;
    // Compute elapsed time since process started (in seconds)
    float elapsed_time = (float) UpTime() - ( (float) starttime / (float) clock_frequence );
    // Compute CPU usage in percentage
    float cpu_usage = 100.0 * ( process_time / elapsed_time );

    return cpu_usage;
 }

// TODO: Read and return the total number of processes
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

// TODO: Read and return the number of running processes
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

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the user associated with a process
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

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
