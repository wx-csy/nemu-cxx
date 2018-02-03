#include "common.h"
#include "CPU/Executer.hpp"

template <typename T>
void Executer<T>::JMP() {
  cpu.regs.eip = *dest;
}

template <typename T>
void Executer<T>::JCC() {
  if (cpu.regs.getcc(decoder.opcode & 0xf)) cpu.regs.eip = *dest;
}

template <typename T>
void Executer<T>::CALL() {
  push(cpu.regs.eip);
  cpu.regs.eip = *dest;
}

template <typename T>
void Executer<T>::RET() {

}

template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

