#include <bits/stdc++.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::atoi;
using std::atol;
using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::cout;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string MemTotal, MemFree, MemAvailable, Buffers, key, line;
  int compareTotal, compareFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> MemTotal;

    std::getline(stream, line);
    std::istringstream linestreamMemFree(line);
    linestreamMemFree >> key >> MemFree;

    std::getline(stream, line);
    std::istringstream linestreamMemAvailable(line);
    linestreamMemAvailable >> key >> MemAvailable;
  }
  
  compareTotal = strcmp(MemTotal.c_str(), "");
  compareFree = strcmp(MemFree.c_str(), "");

  if (compareTotal <= 0 || compareFree <= 0) {
    std::cout << "Invalid total memory or free memory";
  } else if (compareTotal > 0 && compareFree > 0) {
    float memUtil = (std::stof(MemTotal) - std::stof(MemFree)) / std::stof(MemTotal);
    return memUtil;
  }
}

long LinuxParser::UpTime() {
  string key, upTime, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }
  return std::atol(upTime.c_str());
}

long LinuxParser::Jiffies() {
  vector<string> jiffies = CpuUtilization();
  long jiffies_num = 0;
  for (string s : jiffies) {
    jiffies_num += std::atol(s.c_str());
  }
  return jiffies_num;
}

long LinuxParser::ActiveJiffies(int pid) { return 0; }

long LinuxParser::ActiveJiffies() { return LinuxParser::Jiffies() - LinuxParser::IdleJiffies(); }

long LinuxParser::IdleJiffies() {
  vector<string> jiffies = CpuUtilization();
  long idle_jiffies = std::atol(jiffies[3].c_str());
  return idle_jiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> v{};
  string user, nice, system, idle, iowait, irq, softirq, steal, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
  }
  
  v.push_back(user);
  v.push_back(nice);
  v.push_back(system);
  v.push_back(idle);
  v.push_back(iowait);
  v.push_back(irq);
  v.push_back(softirq);
  v.push_back(steal);

  return v;
}

int LinuxParser::TotalProcesses() {
  string name = "processes";
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      break;
    }
  }
  return std::atoi(value.c_str());
}

int LinuxParser::RunningProcesses() {
  string name = "procs_running";
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      break;
    }
  }
  return std::atoi(value.c_str());
}

float LinuxParser::CpuUtilization(int pid) {
  string line, value;
  float result;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  getline(stream, line);
  string s = line;
  std::istringstream buf(s);
  std::istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  float utime = UpTime(pid);
  float stime = std::stof(values[14]);
  float cutime = std::stof(values[15]);
  float cstime = std::stof(values[16]);
  float starttime = std::stof(values[21]);
  float uptime = UpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime;
  float seconds = uptime - (starttime / freq);
  result = 100.0 * ((total_time / freq) / seconds);
  return result;
  std::cout << stime;
}

string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmd);
  }
  return cmd;
}

string LinuxParser::Ram(int pid) {
  string name = "VmSize";
  string key, value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      break;
    }
  }
  value = to_string((std::stof(value) / 1000));
  return value.substr(0, 6);
}

string LinuxParser::Uid(int pid) {
  string name = "Uid", key, value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      break;
    }
  }
  return value;
}

string LinuxParser::User(int pid) {
  string name = "x:" + Uid(pid), key, value, line;
  std::ifstream stream(kPasswordPath);
  while (std::getline(stream, line)) {
    if (line.find(name) != std::string::npos) {
      value = line.substr(0, line.find(":"));
    }
  }
  return value;
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  std::getline(stream, line);
  std::istringstream linestream(line);
  std::istream_iterator<string> beg(linestream), end;
  vector<string> values(beg, end);

  float seconds = float(std::stof(values[13]) / sysconf(_SC_CLK_TCK));
  return seconds;
} 