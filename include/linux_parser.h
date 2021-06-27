#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
  // Paths
  const std::string kProcDirectory{"/proc/"};
  const std::string kCmdlineFilename{"/cmdline"};
  const std::string kCpuinfoFilename{"/cpuinfo"};
  const std::string kStatusFilename{"/status"};
  const std::string kStatFilename{"/stat"};
  const std::string kUptimeFilename{"/uptime"};
  const std::string kMeminfoFilename{"/meminfo"};
  const std::string kVersionFilename{"/version"};
  const std::string kOSPath{"/etc/os-release"};
  const std::string kPasswordPath{"/etc/passwd"};

  // Helpers methods
  std::ifstream& GotoLine(std::ifstream& fileStream, unsigned int lineNumber);

  // System
  float MemoryUtilization(); // Done
  long UpTime(); // Done
  std::vector<int> Pids(); // Done
  int TotalProcesses(); // Done
  int RunningProcesses(); // Done
  std::string OperatingSystem(); // Done
  std::string Kernel(); // Done

  // CPU
  enum CPUStates {
    kUser_ = 0,
    kNice_,
    kSystem_,
    kIdle_,
    kIOwait_,
    kIRQ_,
    kSoftIRQ_,
    kSteal_,
    kGuest_,
    kGuestNice_
  };
  
  std::vector<int> CpuUtilization();
  std::vector<int> CpuUtilization(int pid);
  long Jiffies();
  long ActiveJiffies();
  long ActiveJiffies(int pid);
  long IdleJiffies();

  // Processes
  std::string Command(int pid);
  int Ram(int pid); // Done
  int Uid(int pid); // Done
  std::string User(int pid); // Done
  long int UpTime(int pid);

};  // namespace LinuxParser

#endif