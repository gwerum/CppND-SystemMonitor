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
    _user = LinuxParser::User(_pid);
    _ram_kb = LinuxParser::Ram(_pid);
    _command = LinuxParser::Command(_pid);
    //_cpu_usage = LinuxParser::CpuUtilization(_pid);
}
Process::~Process() {}

// TODO: Return this process's ID
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

// TODO: Return the user (name) that generated this process
string Process::User() { return _user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return _uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& other) const { 
    return (_ram_kb < other._ram_kb ? true : false ); 
}