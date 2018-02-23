#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__
#include "common.h"
#include "devices/mmio_device.hpp"

// 128 MiB memory with 32-bit data bus

struct Memory {
  static const size_t size = 128 * 1024 * 1024;
  uint8_t pmem[size];
  
  mmio_device* mmio_map[size >> 12];
  
  uint32_t paddr_read(paddr_t addr) {
    Assert(addr < size, "Physical address 0x%08x out of range!", addr);
    Assert((addr & 3) == 0, "Physical address 0x%08x unaligned!", addr);
#if HAS_MMIO
    if (__builtin_expect(mmio_map[addr >> 12] != NULL, 0)) {
        mmio_map[addr >> 12]->read(addr,
          *reinterpret_cast<uint32_t*>(pmem + addr));
    }
#endif
    return *reinterpret_cast<uint32_t*>(pmem + addr); 
  }  

  void paddr_write(paddr_t addr, uint32_t data) {
    Assert(addr < size, "Physical address 0x%08x out of range!", addr);
    Assert((addr & 3) == 0, "Physical address 0x%08x unaligned!", addr);
    *reinterpret_cast<uint32_t*>(pmem + addr) = data;     

#if HAS_MMIO
    if (__builtin_expect(mmio_map[addr >> 12] != NULL, 0)) {
      mmio_map[addr >> 12]->write(addr, 
          *reinterpret_cast<uint32_t*>(pmem + addr));
    }
#endif
  }
 
}; 

#endif

