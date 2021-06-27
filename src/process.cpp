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
    _user = LinuxParser::User(_pid);
    _ram_kb = LinuxParser::Ram(_pid);
    _command = LinuxParser::Command(_pid);
    //compute_cpu_utilization_and_uptime(_pid);
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

void Process::compute_cpu_utilization_and_uptime(int pid)
{
    // Read required CPU times spent from proc/pid/stat
    vector<unsigned long> stats = LinuxParser::CpuUtilization(pid);
    // Compute total process time including child processes
    unsigned long utime, stime, cutime, cstime, starttime, total_time;
    utime = stats[10]; // CPU time spent in user code
    stime = stats[11]; // CPU time spent in kernel code
    cutime = stats[12]; // Waited-for children's CPU time spent in user code
    cstime = stats[13]; // Waited-for children's CPU time spent in kernel code
    starttime = stats[18];  // Time when the process started
    total_time = utime + stime + cutime + cstime;
    // Compute elapsed time since process started (in seconds)
    long process_time = total_time / sysconf(_SC_CLK_TCK);
    long process_uptime = starttime / sysconf(_SC_CLK_TCK);
    long elapsed_time = LinuxParser::UpTime() - process_uptime;
    // Compute and store CPU usage in member variable
     _cpu_usage = 100.0F * ( (float) process_time / (float) elapsed_time );
    // Store process uptime (in seconds)
    _uptime = (long) process_uptime;
}