#include <cstdlib>
#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executer<T>::NEMU_TRAP() {
  printf("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
      (cpu.eax == 0 ? "GOOD": "BAD"), cpu.fetcher.eip);
  std::exit(0);
}

template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

