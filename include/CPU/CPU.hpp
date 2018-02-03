#ifndef __CPU_HPP__
#define __CPU_HPP__

#include "common.h"
#include "Memory.hpp"
#include "CPU/Regset.hpp"
#include "CPU/MMU.hpp"

struct CPU {
  Regset regs;
  MMU mmu;
  
  CPU(Memory&);

  template <typename T>
  T instr_fetch() {
    T temp = mmu.vaddr_read<T>(regs.eip);
    regs.eip += sizeof(T);
    return temp;
  }

  uint32_t instr_vfetch(SIZE sz) {
    switch (sz) {
      case SIZE_8:  return instr_fetch<uint8_t>();
      case SIZE_16: return instr_fetch<uint16_t>();
      case SIZE_32: return instr_fetch<uint32_t>();
      default:      panic("Unexpected operand size!");
    }
  }

  uint32_t instr_vfetch_sext(SIZE sz) {
    switch (sz) {
      case SIZE_8:  return (int8_t)instr_fetch<uint8_t>();
      case SIZE_16: return (int16_t)instr_fetch<uint16_t>();
      case SIZE_32: return (int32_t)instr_fetch<uint32_t>();
      default:      panic("Unexpected operand size!");
    }
  }
};

#endif

