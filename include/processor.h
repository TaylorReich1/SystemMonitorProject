#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();
  
  Processor();
  long prev_idle;
  long prev_total;

 private:
};

#endif