#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "linux_parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
    Process(int pid);
    ~Process();
    int Pid() const;                          
    std::string User() const;                  
    std::string Command() const;   
    float CpuUtilization() const;   
    std::string Ram() const;       
    long int UpTime() const;       
    bool operator<(Process const& a) const;

 private:
    int _pid;
    std::string _user;
    std::string _command;
    float _cpu_usage;
    int _ram_kb;
    long int _uptime = 0;

    void compute_cpu_utilization_and_uptime(int pid);
};

#endif