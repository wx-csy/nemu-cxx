#include <cstdlib>
#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executer<T>::NEMU_TRAP() {
  printf("\33[1;31mnemu: HIT %s TRAP\33[0m at $eip = 0x%08x with $eax = 0x%08x\n",
      (cpu.eax == 0 ? "GOOD": "BAD"), cpu.fetcher.eip, cpu.eax);
  
  std::exit(0);
}

template <typename T>
void Executer<T>::NOP() {

}

// TODO: finish IN and OUT
template <typename T>
void Executer<T>::IN() {
  printf("IN(%d): $eax = %08x, $edx = %08x\n", (int) sizeof(T), cpu.eax, cpu.edx);
}

template <typename T>
void Executer<T>::OUT() {
  printf("OUT(%d): $eax = %08x, $edx = %08x\n", (int) sizeof(T), cpu.eax, cpu.edx);
}

template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

