#include "common.h"
#include "CPU.hpp"

void CPU::exec_wrapper() {
  decoder.decode_wrapper();
  eip_before_exec = fetcher.eip;
  switch (decoder.operand_size) {
    case SIZE_8:
      Assert(decoder.current_decode_entry->exec8_helper, 
          "Executor not found!");
      (this->exec8.*(decoder.current_decode_entry->exec8_helper))();
      break;
    case SIZE_16:
      Assert(decoder.current_decode_entry->exec16_helper, 
          "Executor not found!");
      (this->exec16.*(decoder.current_decode_entry->exec16_helper))();
      break;
    case SIZE_32:
      Assert(decoder.current_decode_entry->exec32_helper, 
          "Executor not found!");
      (this->exec32.*(decoder.current_decode_entry->exec32_helper))();
      break;
    default:
      panic("Unexpected operand size");
  }
  mmu.write_operand();

  const uint8_t TIMER_IRQ = 32;
  if (eflags.IF) {
    if (!NO_INTR.test_and_set()) {
      raise_intr(TIMER_IRQ, fetcher.eip);
    }
  }
}

void CPU::raise_intr(uint8_t NO, vaddr_t ret_addr) {
  exec32.push(efl);
  exec32.push(cs);
  exec32.push(ret_addr);
  eflags.IF = 0;
  vaddr_t desc_addr = idtr.base + (NO << 3);
  uint32_t desc_low, desc_high;
  desc_low = mmu.vaddr_read<uint32_t>(desc_addr);
  desc_high = mmu.vaddr_read<uint32_t>(desc_addr + 4);
  vaddr_t int_addr;
  int_addr = ((desc_high >> 16) << 16) | ((desc_low << 16) >> 16);
  
  fetcher.eip = int_addr;
}

