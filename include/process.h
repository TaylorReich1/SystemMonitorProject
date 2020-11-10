#ifndef PROCESS_H
#define PROCESS_H
#include <string>

class Process {
 public:
  Process(int pid);
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization();                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const;  

  // Constrctor
 private:
  int pid;
  std::string user;
  std::string cmd;
  float cpu_utilization;
  std::string ram;
  long int up_time;
};

#endif 