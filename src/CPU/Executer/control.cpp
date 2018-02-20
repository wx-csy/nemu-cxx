#include "common.h"
#include "CPU.hpp"

template <typename T>
void CPU::Executer<T>::JMP() {
  cpu.fetcher.eip = *dest;
}

template <typename T>
void CPU::Executer<T>::JCC() {
  if (cpu.getcc(cpu.opcode & 0xf)) cpu.fetcher.eip = *dest;
}

template <typename T>
void CPU::Executer<T>::CALL() {
  push(cpu.fetcher.eip);
  cpu.fetcher.eip = *dest;
}

template <typename T>
void CPU::Executer<T>::RET() {
  cpu.fetcher.eip = pop();
}

template class CPU::Executer<uint8_t>;
template class CPU::Executer<uint16_t>;
template class CPU::Executer<uint32_t>;

