#include <cstdlib>
#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executor<T>::IN() {
  pio_addr_t port = cpu.decoder.op_immd;
  if (cpu.ports[port] != NULL)
    *dest = cpu.ports[port]->read(port, cpu.decoder.operand_size);
  else
    printf("IN(%d): $eax = %08x, $edx = %08x\n", (int) sizeof(T), cpu.eax, cpu.edx);
}

template <typename T>
void Executor<T>::OUT() {
  pio_addr_t port = cpu.decoder.op_immd;
  if (cpu.ports[port] != NULL)
    cpu.ports[port]->write(port, *dest, cpu.decoder.operand_size);
  else
    printf("OUT(%d): $eax = %08x, $edx = %08x\n", (int) sizeof(T), cpu.eax, cpu.edx);
}

template class Executor<uint8_t>;
template class Executor<uint16_t>;
template class Executor<uint32_t>;

