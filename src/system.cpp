#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "linux_parser.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Returns the system's CPU
Processor& System::Cpu() { return _cpu; }

// Returns a container composed of the system's processes
vector<Process>& System::Processes() {
    _processes.clear();
    vector<int> pids = LinuxParser::Pids();
    // Create process instance for each pid
    for (const auto &pid : pids)
        _processes.emplace_back(Process(pid));
    // Sort in descending order
    std::sort(_processes.rbegin(), _processes.rend());
    return _processes; 
}

// Returns the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Returns the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Returns the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Returns the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Returns the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Returns the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }