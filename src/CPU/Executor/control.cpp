#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executor<T>::JMP() {
  cpu.fetcher.eip = *reinterpret_cast<uint32_t*>(dest);
}

template <typename T>
void Executor<T>::Jcc() {
  if (cpu.getcc(cpu.decoder.opcode & 0xf)) 
    cpu.fetcher.eip = *reinterpret_cast<uint32_t*>(dest);
}

template <typename T>
void Executor<T>::CALL() {
  push(cpu.fetcher.eip);
  cpu.fetcher.eip = *reinterpret_cast<uint32_t*>(dest);
}

template <typename T>
void Executor<T>::RET() {
  cpu.fetcher.eip = pop();
}

template class Executor<uint8_t>;
template class Executor<uint16_t>;
template class Executor<uint32_t>;

