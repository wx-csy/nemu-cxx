#include <cstdlib>
#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executor<T>::NEMU_TRAP() {
  printf("\33[1;31mnemu: HIT %s TRAP\33[0m at $eip = 0x%08x with $eax = 0x%08x\n",
      (cpu.eax == 0 ? "GOOD": "BAD"), cpu.fetcher.eip, cpu.eax);
  
  throw std::runtime_error("nemu trap");
  // std::exit(0);
}

template <typename T>
void Executor<T>::NOP() {

}

// TODO: finish IN and OUT
template <typename T>
void Executor<T>::IN() {
  printf("IN(%d): $eax = %08x, $edx = %08x\n", (int) sizeof(T), cpu.eax, cpu.edx);
}

template <typename T>
void Executor<T>::OUT() {
  printf("OUT(%d): $eax = %08x, $edx = %08x\n", (int) sizeof(T), cpu.eax, cpu.edx);
}

template class Executor<uint8_t>;
template class Executor<uint16_t>;
template class Executor<uint32_t>;

