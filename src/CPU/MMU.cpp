#include "common.h"
#include "CPU/MMU.hpp"
#include "Memory.hpp"

MMU::MMU(Memory& memory):memory(memory) {}

paddr_t MMU::address_translate(vaddr_t addr) {
  return addr;
}

