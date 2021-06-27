#include "processor.h"

// Returns the aggregate CPU utilization
float Processor::Utilization() 
{ 
  // Capture previous idle and total times 
  int prev_idle = _idle;
  int prev_total = _total;
  // Compute new idle and  non-idle times
  compute_idle_and_non_idle_times();
  // Compute deltas
  int total_delta = _total - prev_total;
  int idle_delta = _idle - prev_idle;
  // Compute CPU utilization
  float cpu_usage_percentage = (float)(total_delta - idle_delta) / (float) (total_delta);

  return cpu_usage_percentage; 
}

void Processor::compute_idle_and_non_idle_times()
{
  // Get current CPU times and assign single times
  std::vector<int> cpu_times = LinuxParser::CpuUtilization(); 
  // Get single field values
  int user    = cpu_times[0];
  int nice    = cpu_times[1];
  int system  = cpu_times[2];
  int idle    = cpu_times[3];
  int iowait  = cpu_times[4];
  int irq     = cpu_times[5];
  int softirq = cpu_times[6];
  int steal   = cpu_times[7];
  // Compute idle and non-idle times
  _idle = idle + iowait;
  _non_idle = user + nice + system + irq + softirq + steal;
  _total = _idle + _non_idle;
}