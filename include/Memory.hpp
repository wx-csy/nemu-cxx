#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__
#include "common.h"

// 128 MiB memory with 32-bit data bus

struct Memory {
  static const size_t size = 128 * 1024 * 1024;
  uint8_t pmem[size];
  
  uint32_t paddr_read(paddr_t addr) {
    Assert(addr < size, "Physical address 0x%08x out of range!", addr);
    Assert((addr & 3) == 0, "Physical address 0x%08x unaligned!", addr);
    return *reinterpret_cast<uint32_t*>(pmem + addr); 
  }  

  void paddr_write(paddr_t addr, uint32_t data) {
    Assert(addr < size, "Physical address 0x%08x out of range!", addr);
    Assert((addr & 3) == 0, "Physical address 0x%08x unaligned!", addr);
    *reinterpret_cast<uint32_t*>(pmem + addr) = data;     
  }
 
}; 

#endif

