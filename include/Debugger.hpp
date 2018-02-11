#ifndef __DEBUGGER_HPP__
#define __DEBUGGER_HPP__

#include <map>
#include <sstream>
#include <string>
#include "common.h"
#include "Memory.hpp"
#include "CPU.hpp"

struct Debugger {
  CPU& cpu;
  Memory& memory;

  Debugger(CPU& cpu, Memory& memory);

  void load_default_image();
   
  struct cmd_entry {
    const char* description;
    void (Debugger::*handler)(std::istringstream&);
  };

  void cmd_help(std::istringstream& args);
  void cmd_q(std::istringstream& args);
  void cmd_c(std::istringstream& args);
  void cmd_p(std::istringstream& args);

  static const std::map<std::string, cmd_entry> cmd_table;

  void mainloop();
};

#endif

