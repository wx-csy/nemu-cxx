#include "common.h"
#include "CPU.hpp"

void CPU::exec_wrapper() {
  decode_wrapper();
  eip_before_exec = fetcher.eip;
  switch (operand_size) {
    case SIZE_8:
      Assert(current_decode_entry->exec8_helper, "Executer not found!");
      (this->exec8.*(current_decode_entry->exec8_helper))();
      break;
    case SIZE_16:
      Assert(current_decode_entry->exec16_helper, "Executer not found!");
      (this->exec16.*(current_decode_entry->exec16_helper))();
      break;
    case SIZE_32:
      Assert(current_decode_entry->exec32_helper, "Executer not found!");
      (this->exec32.*(current_decode_entry->exec32_helper))();
      break;
    default:
      // TODO: emit more hints
      panic("Unexpected operand size");
  }
  mmu.write_operand();
}

