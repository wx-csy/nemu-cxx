#include "common.h"
#include "CPU.hpp"
#include "CPU/MMU.hpp"

paddr_t MMU::address_translate(vaddr_t addr) {
  return addr;
}

MMU::MMU(Memory& memory) : memory(memory) {}

template <>
uint32_t MMU::vaddr_read<uint32_t>(vaddr_t addr) {
  if ((addr & 3) == 0) return memory[address_translate(addr)];
  else 
    return memory[address_translate(addr & -4)] >> ((addr & 3) * 8) |
      memory[address_translate((addr & -4) + 4)] << ((-addr & 3) * 8); 
}

template <>
uint16_t MMU::vaddr_read<uint16_t>(vaddr_t addr) {
  if ((addr & 3) != 3) return (memory[address_translate(addr & -4)] 
    >> ((addr & 3) * 8)) & 0xffff;
  else return (memory[address_translate(addr - 3)] >> 24 | 
    memory[address_translate(addr + 1)] << 8) & 0xffff; 
}

template <>
uint8_t MMU::vaddr_read<uint8_t>(vaddr_t addr) {
  return ((memory[address_translate(addr & -4)]) 
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
  if ((addr & 3) == 0) memory[address_translate(addr)] = data;
  else {
    (memory[address_translate(addr & -4)] 
      &= (0xffffffff >> ((addr & 3) * 8)))
      |= data << ((addr & 3) * 8);
    (memory[address_translate((addr & -4) + 4)]
      &= (0xffffffff << ((-addr & 3) * 8)))
      |= data >> ((-addr & 3) * 8);    
  }
}

template <>
void MMU::vaddr_write<uint16_t>(vaddr_t addr, uint16_t data) {
  if ((addr & 3) != 3) {
    (memory[address_translate(addr & -4)] 
      &= ~(0xffff << ((addr & 3) * 8)))
      |= data << ((addr * 3) * 8);
  } else {
    (memory[address_translate(addr - 3)] &= 0x00ffffff) |= data << 24;
    (memory[address_translate(addr + 1)] &= 0xffffff00) |= data >> 8;
  }
}

template <>
void MMU::vaddr_write<uint8_t>(vaddr_t addr, uint8_t data) {
  (memory[address_translate(addr & -4)] 
    &= ~(0xff << ((addr & 3) * 8))) 
    |= data << ((addr & 3) * 8);
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

