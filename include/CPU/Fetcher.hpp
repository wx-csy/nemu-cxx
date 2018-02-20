#ifndef __CPU_FETCHER_HPP__
#define __CPU_FETCHER_HPP__

#include "common.h"
#include "CPU/MMU.hpp"

struct Fetcher {
  MMU& mmu;  
  vaddr_t eip;

  Fetcher(MMU& mmu) : mmu(mmu) {}

  template <typename T>
  T fetch() {
    T temp = mmu.vaddr_read<T>(eip);
    eip += sizeof(T);
    return temp;
  }

  uint32_t fetch(SIZE sz) {
    switch (sz) {
      case SIZE_8:  return fetch<uint8_t>();
      case SIZE_16: return fetch<uint16_t>();
      case SIZE_32: return fetch<uint32_t>();
      default:      panic("Unexpected opreand size!");
    }
  }
};

#endif

