#include "common.h"
#include "CPU.hpp"
#include "Memory.hpp"

CPU::CPU(Memory& memory, Ports& ports) :
  memory(memory), ports(ports),
  efl(0x00000002), mmu(memory), fetcher(mmu),
  decoder(*this),
  exec32(*this), exec16(*this), exec8(*this)
{
  NO_INTR.test_and_set();
  fetcher.eip = ENTRY_START; 
}

