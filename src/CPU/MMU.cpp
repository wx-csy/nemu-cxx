#include "common.h"
#include "CPU.hpp"
#include "CPU/MMU.hpp"

static inline bool is_cross_page(vaddr_t addr, SIZE size) {
  return ((addr ^ (addr + size - 1)) >> 12) != 0;
}

MMU::MMU(Memory& memory) : memory(memory), CR0(0), CR3(0) {}

paddr_t MMU::address_translate(vaddr_t addr) {
  if (!cr0.present) return addr;

  union {
    struct {
      uint32_t offset:12;
      uint32_t page:10;
      uint32_t dir:10;
    };
    vaddr_t addr;
  } vaddr;
  vaddr.addr = addr;
  union pte {
    struct {
      uint32_t present:1;
      uint32_t unused:11;
      uint32_t page_addr:20;
    };
    uint32_t pte_u32;
  } dir_entry, page_entry;
  
  // Note: accessed and dirty bits are not implemented!
  paddr_t dir_entry_addr = (cr3.pbtr << 12) | (vaddr.dir << 2);
  dir_entry.pte_u32 = memory.paddr_read<uint32_t>(dir_entry_addr);
  Assert(dir_entry.present, "Page directory not present.\n"
      "(on translating virtual address 0x%x)\n", addr);
  
  paddr_t page_entry_addr = (dir_entry.page_addr << 12) | (vaddr.page << 2);
  page_entry.pte_u32 = memory.paddr_read<uint32_t>(page_entry_addr);
  Assert(page_entry.present, "Page table not present.\n"
      "(On translating virtual address 0x%x)\n", addr);
  
  paddr_t paddr = (page_entry.page_addr << 12) | (vaddr.offset);
  return paddr;
}

template <typename T>
T MMU::vaddr_read(vaddr_t addr) {
  if (!is_cross_page(addr, (SIZE)sizeof(T))) {
    return memory.paddr_read<T>(address_translate(addr));
  } else {
    union {
      T data;
      uint8_t bytes[sizeof(T)];
    };
    for (uint32_t i = 0; i < sizeof(T); i++) 
      bytes[i] = memory.paddr_read<uint8_t>(address_translate(addr + i));
    return data;
  }
}

template uint32_t MMU::vaddr_read<uint32_t>(vaddr_t addr);
template uint16_t MMU::vaddr_read<uint16_t>(vaddr_t addr);
template uint8_t MMU::vaddr_read<uint8_t>(vaddr_t addr);

uint32_t MMU::vaddr_read(vaddr_t addr, SIZE size) {
  switch (size) {
    case SIZE_32: return vaddr_read<uint32_t>(addr);
    case SIZE_16: return vaddr_read<uint16_t>(addr);
    case SIZE_8:  return vaddr_read<uint8_t>(addr);
    default: panic("Unexpected operand size!");
  } 
  return 0;
}

template <typename T>
void MMU::vaddr_write(vaddr_t addr, T data) {
  if (!is_cross_page(addr, (SIZE)sizeof(T))) {
    memory.paddr_write<T>(addr, data);
  } else {
    union {
      T data;
      uint8_t bytes[sizeof(T)];
    } u;
    u.data = data;
    for (uint32_t i = 0; i < sizeof(T); i++) 
      memory.paddr_write<uint8_t>(address_translate(addr + i), 
          u.bytes[i]);
  }
}

template void MMU::vaddr_write<uint32_t>(vaddr_t addr, uint32_t data);
template void MMU::vaddr_write<uint16_t>(vaddr_t addr, uint16_t data);
template void MMU::vaddr_write<uint8_t>(vaddr_t addr, uint8_t data);

void MMU::vaddr_write(vaddr_t addr, uint32_t data, SIZE size) {
  switch (size) {
    case SIZE_32: vaddr_write<uint32_t>(addr, data); return;
    case SIZE_16: vaddr_write<uint16_t>(addr, data); return;
    case SIZE_8:  vaddr_write<uint8_t>(addr, data); return;
    default: panic("Unexpected operand size!");
  }
}

void* MMU::memory_access(vaddr_t addr, SIZE sz) {
  if (!is_cross_page(addr, sz))
    return (memory.pmem) + address_translate(addr);
  else {
    Assert(!write_operand_flag, "Access memory more than once!");
    write_operand_flag = true;
    operand_addr = addr;
    operand_size = sz;
    operand_tmp = vaddr_read(addr, sz);
    return &operand_tmp;
  }
}

void MMU::write_operand() {
  if (write_operand_flag) {
    vaddr_write(operand_addr, operand_tmp, operand_size);
  }
  write_operand_flag = false;
}

