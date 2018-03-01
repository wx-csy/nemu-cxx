#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__
#include "common.h"

// 128 MiB memory

struct Memory {
  static const size_t size = 128 * 1024 * 1024;
  uint8_t pmem[size + 8];
  
  template <typename T>
  T paddr_read(paddr_t addr) {
    return *reinterpret_cast<T*>(pmem + addr);
  }

  template <typename T>
  void paddr_write(paddr_t addr, T data) {
    *reinterpret_cast<T*>(pmem + addr) = data;
  }
}; 

#endif

