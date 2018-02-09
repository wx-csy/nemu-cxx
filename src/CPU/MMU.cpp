#include "common.h"
#include "CPU.hpp"

paddr_t CPU::address_translate(vaddr_t addr) {
  return addr;
}

void* CPU::memory_access(vaddr_t addr, SIZE sz) {
  return memory.pmem + address_translate(addr);
}

void CPU::write_operand() {

}

