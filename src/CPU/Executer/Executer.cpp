#include "common.h"
#include "CPU.hpp"

template <typename T>
Executer<T>::Executer(CPU& cpu) : 
  cpu(cpu),
  dest(reinterpret_cast<T*&>(cpu.decoder.dest)),
  src(reinterpret_cast<T*&>(cpu.decoder.src)) {}

template <typename T>
void Executer<T>::push(T x) {
  cpu.esp -= 4;
  cpu.mmu.vaddr_write<uint32_t>(cpu.esp, x);
}

template <typename T>
T Executer<T>::pop() {
  T temp = cpu.mmu.vaddr_read<uint32_t>(cpu.esp);
  cpu.esp += 4;
  return temp;
}

template class Executer<uint32_t>;
template class Executer<uint16_t>;
template class Executer<uint8_t>;

