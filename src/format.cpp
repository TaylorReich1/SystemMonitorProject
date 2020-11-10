#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
  long HH = seconds / 60 / 60;
  long MM = (seconds / 60) % 60;
  long SS = seconds % 60;
  string elapsedTime = std::to_string(HH) + ":" + std::to_string(MM) + ":" + std::to_string(SS);
  return elapsedTime;
}