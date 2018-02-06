#include "common.h"
#include "CPU.hpp"
#include "Memory.hpp"

CPU::CPU(Memory& memory) :
  memory(memory), 
  exec32(*this), exec16(*this), exec8(*this) {}

