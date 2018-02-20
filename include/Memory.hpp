#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__
#include "common.h"

// 128 MiB memory with 32-bit data bus

struct Memory {
private:
  static const size_t size = 128 * 1024 * 1024;
  uint32_t pmem[size / 4];

public:
  uint32_t& operator[] (paddr_t addr) {
    Assert((addr & 3) == 0, "Physical address 0x%x unaligned!", addr);
    return pmem[addr >> 2];
  }  
}; 

#endif

