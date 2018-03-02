#ifndef __CPU_MMU_HPP__
#define __CPU_MMU_HPP__

#include "common.h"
#include "Memory.hpp"

struct MMU {
  Memory& memory;
  paddr_t address_translate(vaddr_t addr);
 
private: 
  bool write_operand_flag = false;
  uint32_t operand_tmp;
  vaddr_t operand_addr;
  SIZE operand_size;

public:
  MMU(Memory& memory);
  
  union {
    struct {
      uint32_t unused:31;
      uint32_t present:1;
    } cr0;
    uint32_t CR0;
  };

  union {
    struct {
      uint32_t unused:12;
      uint32_t pbtr:20;
    } cr3;
    uint32_t CR3;
  };

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

