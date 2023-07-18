#include "linux_parser.h"
#include "ncurses_display.h"
#include "system.h"
#include <iostream>

int main() {
  System system;
  NCursesDisplay::Display(system);

  // float memory = LinuxParser::MemoryUtilization();
  // std::cout.precision(9);
  // std::cout << "Memory: " << memory << " MB" << std::endl;
}