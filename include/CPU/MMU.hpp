#ifndef __CPU_MMU_HPP__
#define __CPU_MMU_HPP__

#include "common.h"
#include "Memory.hpp"

struct MMU {
private:
  Memory& memory;
  paddr_t address_translate(vaddr_t addr);

public:
  MMU(Memory& memory);
  
  uint32_t vaddr_read(vaddr_t addr, SIZE size);
  void vaddr_write(vaddr_t addr, uint32_t data, SIZE size);
  
  template <typename T>
  T vaddr_read(vaddr_t addr);
  template <typename T>
  void vaddr_write(vaddr_t addr, T data);

  void* memory_access(vaddr_t addr, SIZE sz);
  void write_operand();
};

#endif

