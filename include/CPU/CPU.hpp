#ifndef __CPU_HPP__
#define __CPU_HPP__

#include "common.h"
#include "Memory.hpp"
#include "CPU/Regset.hpp"
#include "CPU/MMU.hpp"

struct CPU {
  Regset regs;
  MMU mmu;
  
  CPU(Memory&);
};

#endif

