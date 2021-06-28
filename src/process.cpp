#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Define constructor and destructor
Process::Process(int pid) 
: _pid( pid )
{
    // Compute process variables
    _user = LinuxParser::User(pid);
    _ram_kb = LinuxParser::Ram(pid);
    _command = LinuxParser::Command(pid);
    _cpu_usage = compute_cpu_utilization(pid);
    _uptime = LinuxParser::UpTime(pid);
}

Process::~Process() {}

// Returns this process's ID
int Process::Pid() { return _pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return _cpu_usage; }

// TODO: Return the command that generated this process
string Process::Command() { return _command; }

// TODO: Return this process's memory utilization
string Process::Ram() {
    float ram_mb = (float) _ram_kb / (float) 1024;
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << ram_mb;
    return stream.str(); 
}

// Returns the user (name) that generated this process
string Process::User() { return _user; }

// Returns the age of this process (in seconds)
long Process::UpTime() { return _uptime; }

// Overloads the less operator according to cpu utilization
bool Process::operator<(Process const& rhs) const { 
  return this->_cpu_usage < rhs._cpu_usage;
}

// Overloads the greater operator according to cpu utilization
bool Process::operator>(Process const& rhs) const { 
  return this->_cpu_usage > rhs._cpu_usage;
}

// Computes CPU utilization from active Jiffies and returns it
float Process::compute_cpu_utilization(int pid)
{
    // Get active Jiffies
    float active_jiffies_process = static_cast<float>(LinuxParser::ActiveJiffies(pid));
    float active_jiffies_processor = static_cast<float>(LinuxParser::ActiveJiffies());
    // Compute and store CPU usage in member variable
    return (active_jiffies_process / active_jiffies_processor);
}