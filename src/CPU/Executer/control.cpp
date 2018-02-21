#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executer<T>::JMP() {
  cpu.fetcher.eip = *reinterpret_cast<uint32_t*>(dest);
}

template <typename T>
void Executer<T>::Jcc() {
  if (cpu.getcc(cpu.decoder.opcode & 0xf)) 
    cpu.fetcher.eip = *reinterpret_cast<uint32_t*>(dest);
}

template <typename T>
void Executer<T>::CALL() {
  push(cpu.fetcher.eip);
  cpu.fetcher.eip = *reinterpret_cast<uint32_t*>(dest);
}

template <typename T>
void Executer<T>::RET() {
  cpu.fetcher.eip = pop();
}

template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

