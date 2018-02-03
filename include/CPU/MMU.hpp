#ifndef __MEMCTL_HPP__
#define __MEMCTL_HPP__

#include "common.h"
#include "Memory.hpp"

struct MMU {
  Memory &memory;

  MMU(Memory &memory);  

  paddr_t address_translate(vaddr_t);
  
  template <typename T>
  T vaddr_read(vaddr_t addr) {
    return memory.paddr_read<T>(addr);
  }

  template <typename T>
  void vaddr_write(vaddr_t addr, T data) {
    memory.paddr_write<T>(addr, data);
  }
};

#endif

