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

template <typename T>
void Executor<T>::LIDT() {
  Assert(sizeof(T) == 4, "Unimplemented");
  cpu.idtr.limit = cpu.mmu.vaddr_read<uint16_t>(*dest);
  cpu.idtr.base = cpu.mmu.vaddr_read<uint32_t>(*dest + 2);
}

template <typename T>
void Executor<T>::INT() {
  assert(sizeof(T) == 1);

  cpu.raise_intr(*dest, cpu.fetcher.eip);
}

template <typename T>
void Executor<T>::IRET() {
  cpu.fetcher.eip = pop();
  cpu.cs = pop();
  cpu.efl = pop();
}

template <typename T>
void Executor<T>::MOV_r2cr() {
  switch (cpu.decoder.ModRM.reg_op) {
    case 0: cpu.mmu.CR0 = *src; return ;
    case 3: cpu.mmu.CR3 = *src; 
#ifdef HAS_TLB
            cpu.mmu.TLB_flush();
#endif
            return ;
    default: panic("Register not supported!");
  }
}

template <typename T>
void Executor<T>::MOV_cr2r() {
  switch (cpu.decoder.ModRM.reg_op) {
    case 0: *dest = cpu.mmu.CR0; return ;
    case 3: *dest = cpu.mmu.CR3; return ;
    default: panic("Register not supported!");
  }
}

template class Executor<uint8_t>;
template class Executor<uint16_t>;
template class Executor<uint32_t>;

