#include "common.h"
#include "CPU.hpp"
#include "Memory.hpp"

CPU::CPU(Memory& memory) :
  memory(memory), 
  efl(0x00000002), eip(ENTRY_START),
  exec32(*this), exec16(*this), exec8(*this)
{
  
}

