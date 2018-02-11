#ifndef __DEBUGGER_REGS_H__
#define __DEBUGGER_REGS_H__

#include <map>
#include <string>
#include <utility>
#include "CPU.hpp"
extern std::map <std::string, uint16_t> regid;

std::pair<void*, SIZE> get_reg_info(CPU& cpu, uint16_t id);

#endif

