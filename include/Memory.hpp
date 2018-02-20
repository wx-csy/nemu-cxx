#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__
#include "common.h"

// 128 MiB memory with 32-bit data bus

struct Memory {
  static const size_t size = 128 * 1024 * 1024;
  uint8_t pmem[size];
  
  uint32_t& operator[] (paddr_t addr) {
    Assert((addr & 3) == 0, "Physical address 0x%x unaligned!", addr);
    return *reinterpret_cast<uint32_t*>(pmem + addr);
  }  
}; 

#endif

