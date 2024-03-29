#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>
#include <vector>

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

  // Methods to compute system states
  float MemoryUtilization(); // Done
  long UpTime(); // Done
  std::vector<int> Pids(); // Done
  int TotalProcesses(); // Done
  int RunningProcesses(); // Done
  std::string OperatingSystem(); // Done
  std::string Kernel(); // Done
  std::vector<std::string> CpuUtilization(); // Done

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

  // Computing Jiffies
  long Jiffies();
  long IdleJiffies();
  long ActiveJiffies();
  long ActiveJiffies(int pid);
  
  // Methods to compute process state
  std::string Command(int pid); // Done
  int Ram(int pid); // Done
  int Uid(int pid); // Done
  std::string User(int pid); // Done
  long UpTime(int pid); // Done

};  // namespace LinuxParser

#endif