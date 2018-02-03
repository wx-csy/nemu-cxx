#ifndef __REGSET_HPP__
#define __REGSET_HPP__

#include "common.h"

struct Regset {
  union {
    struct {
      union {uint32_t eax; uint16_t ax; struct {uint8_t al, ah;};};
      union {uint32_t ecx; uint16_t cx; struct {uint8_t cl, ch;};};
      union {uint32_t edx; uint16_t dx; struct {uint8_t dl, dh;};};
      union {uint32_t ebx; uint16_t bx; struct {uint8_t bl, bh;};};
      union {uint32_t esp; uint16_t sp;};
      union {uint32_t ebp; uint16_t bp;};
      union {uint32_t esi; uint16_t si;};
      union {uint32_t edi; uint16_t di;};
    };
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];
  };

  union {
    struct {
      uint32_t CF:1, _reserved_0:1, PF:1, _reserved_1:1, AF:1, 
               _reserved_2:1, ZF:1, SF:1, TF:1, IF:1, DF:1, OF:1, IOPL:2,
               NT:1, _reserved_3:1, RF:1, VM:1, _reserved_4:14;
    } eflags;
    uint32_t efl;
  };

  vaddr_t eip;

  void* get_reg_ptr(unsigned id, SIZE sz) {
    Assert(id >= 0 && id < 8, "Register id out of range!");
    switch (sz) {
      case SIZE_8:  return &gpr[id >> 1]._8[id & 1];
      case SIZE_16: return &gpr[id]._16;
      case SIZE_32: return &gpr[id]._32;
      default:      panic("Unexpected register size!");
    }
  }
};

#endif

