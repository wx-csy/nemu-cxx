#include "common.h"
#include "CPU.hpp"
#include "CPU/MMU.hpp"

paddr_t MMU::address_translate(vaddr_t addr) {
  return addr;
}

MMU::MMU(Memory& memory) : memory(memory) {}

template <>
uint32_t MMU::vaddr_read<uint32_t>(vaddr_t addr) {
  if ((addr & 3) == 0) return memory.paddr_read(address_translate(addr));
  else return memory.paddr_read(address_translate(addr & -4)) >> 
    ((addr & 3) * 8) 
      | memory.paddr_read(address_translate((addr & -4) + 4)) << 
    ((-addr & 3) * 8); 
}

template <>
uint16_t MMU::vaddr_read<uint16_t>(vaddr_t addr) {
  if ((addr & 3) != 3) 
    return (memory.paddr_read(address_translate(addr & -4))
      >> ((addr & 3) * 8)) & 0xffff;
  else return (memory.paddr_read(address_translate(addr - 3)) >> 24 | 
    memory.paddr_read(address_translate(addr + 1)) << 8) & 0xffff; 
}

template <>
uint8_t MMU::vaddr_read<uint8_t>(vaddr_t addr) {
  return ((memory.paddr_read(address_translate(addr & -4))) 
    >> ((addr & 3) * 8)) & 0xff; 
}

uint32_t MMU::vaddr_read(vaddr_t addr, SIZE size) {
  switch (size) {
    case SIZE_32: return vaddr_read<uint32_t>(addr);
    case SIZE_16: return vaddr_read<uint16_t>(addr);
    case SIZE_8:  return vaddr_read<uint8_t>(addr);
    default: panic("Unexpected operand size!");
  } 
}

template <>
void MMU::vaddr_write<uint32_t>(vaddr_t addr, uint32_t data) {
  if ((addr & 3) == 0) memory.paddr_write(address_translate(addr), data);
  else {
    paddr_t paddr;
    uint32_t temp;
    paddr = address_translate(addr & -4);
    temp = memory.paddr_read(paddr) & (0xffffffff >> ((addr & 3) * 8));
    temp |= data << ((addr & 3) * 8);
    memory.paddr_write(paddr, temp);
    paddr = address_translate((addr & -4) + 4);
    temp = memory.paddr_read(paddr) & (0xffffffff << ((-addr & 3) * 8));
    temp |= data >> ((-addr & 3) * 8);
    memory.paddr_write(paddr, temp);
  }
}

template <>
void MMU::vaddr_write<uint16_t>(vaddr_t addr, uint16_t data) {
  paddr_t paddr;
  uint32_t temp;
  if ((addr & 3) != 3) {
    paddr = address_translate(addr & -4); 
    temp = memory.paddr_read(paddr) & ~(0xffff << ((addr & 3) * 8));
    temp |= data << ((addr * 3) * 8);
    memory.paddr_write(paddr, temp);
  } else {
    paddr = address_translate(addr - 3);
    temp = memory.paddr_read(paddr) & 0x00ffffff;
    temp |= data << 24;
    memory.paddr_write(paddr, temp);
    paddr = address_translate(addr + 1);
    temp = memory.paddr_read(paddr) & 0xffffff00;
    temp |= data >> 8;
    memory.paddr_write(paddr, temp);
  }
}

template <>
void MMU::vaddr_write<uint8_t>(vaddr_t addr, uint8_t data) {
  paddr_t paddr = address_translate(addr & -4);
  uint32_t temp = memory.paddr_read(paddr); 
  temp &= ~(0xff << ((addr & 3) * 8));
  temp |= data << ((addr & 3) * 8);
  memory.paddr_write(paddr, temp);  
}

void MMU::vaddr_write(vaddr_t addr, uint32_t data, SIZE size) {
  switch (size) {
    case SIZE_32: vaddr_write<uint32_t>(addr, data); return;
    case SIZE_16: vaddr_write<uint16_t>(addr, data); return;
    case SIZE_8:  vaddr_write<uint8_t>(addr, data); return;
    default: panic("Unexpected operand size!");
  }
}

void* MMU::memory_access(vaddr_t addr, SIZE sz) {
  return (memory.pmem) + address_translate(addr);
}

void MMU::write_operand() {

}

