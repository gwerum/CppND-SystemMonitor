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
    int Pid();                          
    std::string User();              
    std::string Command();   
    float CpuUtilization();
    std::string Ram();       
    long UpTime();       
    bool operator<(Process const& a) const;

 private:
    int _pid = 0;
    std::string _user = "unknown";
    std::string _command = "unknown";
    float _cpu_usage = 0.0;
    int _ram_kb = 0;
    long _uptime = 0;

    void compute_cpu_utilization_and_uptime(int pid);
};

#endif