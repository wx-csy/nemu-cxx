#ifndef __DEBUGGER_HPP__
#define __DEBUGGER_HPP__

#include <map>
#include <set>
#include <sstream>
#include <string>
#include "common.h"
#include "Memory.hpp"
#include "CPU.hpp"
#include "Debugger/expr.h"

struct Debugger {
  CPU& cpu;
  Memory& memory;

  Debugger(CPU& cpu, Memory& memory);

  void load_image(const char* path);
  void load_default_image();
   
  struct cmd_entry {
    const char* description;
    void (Debugger::*handler)(std::istringstream&);
  };

  void cmd_help(std::istringstream& args);
  void cmd_q(std::istringstream& args);
  void cmd_c(std::istringstream& args);
  void cmd_si(std::istringstream& args);
  void cmd_p(std::istringstream& args);
  void cmd_b(std::istringstream& args);
  void cmd_i(std::istringstream& args);

  static const std::map<std::string, cmd_entry> cmd_table;
   
  expr build_expr(token_iter l, token_iter r);

  std::set<uint32_t> breakpoints;

  void exec_wrapper();
  void mainloop();
};

#endif

