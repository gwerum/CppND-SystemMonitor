#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include "linux_parser.h"

class Processor {
 public:
  float Utilization();
 private:
  // Calls linux parser to parse /proc/stat and computes idle times
  void compute_idle_and_non_idle_times();
  // Member variables to store idle and non-idle times
  int _idle = 0;
  int _non_idle = 0;
  int _total = 0; 
};

#endif