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
vector<string> LinuxParser::CpuUtilization() { return {}; }

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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
int LinuxParser::Uid(int pid) {
  string key;
  int uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid));
  if (filestream.is_open()) {
    GotoLine(filestream, 9); // line 9 format: "Uid:	0	0	0	0"
    filestream >> key >> uid;
  }
  return uid;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  int uid = Uid(pid);
  string line, p_user, x;
  int p_uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {    // line format: root:x:0:0:root:/root:/bin/bash
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> p_user >> x >> p_uid) {
        if (p_uid == uid)
          return p_user;
      }
    }
  }
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
