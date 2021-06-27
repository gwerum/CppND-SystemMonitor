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
    compute_cpu_utilization_and_uptime();
}

Process::~Process() {}

// Returns this process's ID
int Process::Pid() const { return _pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return _cpu_usage; }

// TODO: Return the command that generated this process
string Process::Command() const { return _command; }

// TODO: Return this process's memory utilization
string Process::Ram() const {
    float ram_mb = (float) _ram_kb / (float) 1024;
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << ram_mb;
    return stream.str(); 
}

// Returns the user (name) that generated this process
string Process::User() const { return _user; }

// Returns the age of this process (in seconds)
long int Process::UpTime() const { return _uptime; }

// Overloads the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const { 
    return (_ram_kb < other._ram_kb ? true : false ); 
}

void Process::compute_cpu_utilization_and_uptime()
{
    // Read required CPU times spent from proc/pid/stat
    std::vector<unsigned long> stats = LinuxParser::CpuUtilization(_pid);
    // Compute total process time including child processes
    long clock_frequence = sysconf(_SC_CLK_TCK);
    unsigned long utime, stime, cutime, cstime, starttime, total_time;
    utime = stats[10]; // CPU time spent in user code
    stime = stats[11]; // CPU time spent in kernel code
    cutime = stats[12]; // Waited-for children's CPU time spent in user code
    cstime = stats[13]; // Waited-for children's CPU time spent in kernel code
    starttime = stats[18];  // Time when the process started
    total_time = utime + stime + cutime + cstime;
    float process_time = (float) total_time / (float) clock_frequence;
    // Compute elapsed time since process started (in seconds)
    float elapsed_time = (float) LinuxParser::UpTime() - ( (float) starttime / (float) clock_frequence );
    // Compute and store CPU usage in member variable
    _cpu_usage = (float) (100.0) * ( process_time / elapsed_time );
    // Compute uptime of this process
    _uptime = (long int) starttime / (long int) clock_frequence;
}