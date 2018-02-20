#include "common.h"
#include "CPU.hpp"

Decoder::Decoder(CPU& cpu) : cpu(cpu), fetcher(cpu.fetcher) {}

Decoder::Decoder(CPU& cpu, Fetcher& fetcher)
  : cpu(cpu), fetcher(fetcher) {}


void Decoder::ModRM_decode() {
  reinterpret_cast<uint8_t&>(ModRM) = cpu.fetcher.fetch<uint8_t>();
}
 
void* Decoder::ModRM_get_reg_ptr() {
  return cpu.get_reg_ptr(ModRM.reg_op, operand_size);
}

vaddr_t Decoder::ModRM_get_mem_vaddr() {
  assert(ModRM.mod != 3);
    
  vaddr_t addr = 0;
  int32_t disp = 0;
  SIZE disp_size = SIZE_32;
  int base_reg = -1, index_reg = -1, scale = 0;
  
  if (ModRM.R_M == 4) {
    struct {
      uint8_t base  :3;
      uint8_t index :3;
      uint8_t ss    :2;
    } sib;
    reinterpret_cast<uint8_t&>(sib) = fetcher.fetch<uint8_t>();
    base_reg = sib.base;
    scale = sib.ss;
    if (sib.index != 4) index_reg = sib.index;
  } else {
    base_reg = ModRM.R_M;
  } 

  if (ModRM.mod == 0) {
    if (base_reg == 5) base_reg = -1; else disp_size = SIZE_NONE;
  } else if (ModRM.mod == 1) disp_size = SIZE_8;

  if (disp_size) {
    disp = fetcher.fetch_s(disp_size);
    addr += disp;
  }
 
  if (base_reg != -1) 
    addr += cpu.gpr[base_reg]._32;
  
  if (index_reg != -1) 
    addr += cpu.gpr[index_reg]._32 << scale;
  
  return addr;
} 
  
void* Decoder::ModRM_get_rm_ptr() {
  if (ModRM.mod == 3) {
    return cpu.get_reg_ptr(ModRM.R_M, operand_size);
  } else {
    return cpu.mmu.memory_access(ModRM_get_mem_vaddr(), operand_size); 
  }
}

void* Decoder::ModRM_get_rm_ptr(SIZE sz) {
  if (ModRM.mod == 3) {
    return cpu.get_reg_ptr(ModRM.R_M, sz);
  } else {
    return cpu.mmu.memory_access(ModRM_get_mem_vaddr(), sz);
  }
}

inline void* Decoder::decode_op_immd() {
  op_immd = fetcher.fetch_u(operand_size);
  return &op_immd;
}

inline void* Decoder::decode_op_simmd() {
  op_immd = fetcher.fetch_s(operand_size);
  return &op_immd;
}

inline void* Decoder::decode_op_regA() {
  return &cpu.eax;
}

inline void* Decoder::decode_op_reg() {
  return cpu.get_reg_ptr(opcode & 0x7, operand_size);
}

inline void* Decoder::decode_op_offset() {
  uint32_t addr = fetcher.fetch_s(operand_size);
  return cpu.mmu.memory_access(addr, operand_size);
}

// reg -> r/m
void Decoder::decode_G2E() {
  ModRM_decode();
  dest = ModRM_get_rm_ptr();
  src = ModRM_get_reg_ptr();
}

// r/m -> reg
void Decoder::decode_E2G() {
  ModRM_decode();
  dest = ModRM_get_reg_ptr();
  src = ModRM_get_rm_ptr();
}

// r/m (byte) -> reg
void Decoder::decode_Eb2G() {
  ModRM_decode();
  dest = ModRM_get_reg_ptr();
  src = ModRM_get_rm_ptr(SIZE_8); 
}

void Decoder::decode_Ew2G() {
  ModRM_decode();
  dest = ModRM_get_reg_ptr();
  src = ModRM_get_rm_ptr(SIZE_16);
}

// r/m (mem only) -> reg (for lea only)
void Decoder::decode_M2G_lea() {
  ModRM_decode();
  dest = ModRM_get_reg_ptr(); 
  Assert(ModRM.mod != 3, "Invalid lea operand! (#UD)");
  op_immd = ModRM_get_mem_vaddr(); 
  src = &op_immd;
}

void Decoder::decode_I2a() {
  dest = decode_op_regA();
  src = decode_op_immd();
}

void Decoder::decode_I_E2G() {
  decode_E2G();
  decode_op_immd();
}

void Decoder::decode_I2E() {
  ModRM_decode();
  dest = ModRM_get_rm_ptr();
  src = decode_op_immd();
}

void Decoder::decode_I2r() {
  dest = decode_op_reg();
  src = decode_op_immd();
}

void Decoder::decode_O2a() {
  dest = decode_op_regA();
  src = decode_op_offset();
}

void Decoder::decode_a2O() {
  dest = decode_op_offset();
  src = decode_op_regA();
}

void Decoder::decode_I() {
  dest = decode_op_immd();
}

void Decoder::decode_r() {
  dest = decode_op_reg();
}

void Decoder::decode_E() {
  ModRM_decode();
  dest = ModRM_get_rm_ptr();
}

void Decoder::decode_I_test() {
  src = decode_op_immd();
}

void Decoder::decode_twobyte_escape() {
  opcode = fetcher.fetch<uint8_t>();
  current_decode_entry = &twobyte_opcode_table[opcode];
}

void Decoder::decode_prefix() {
  opcode = fetcher.fetch<uint8_t>();
  current_decode_entry = &opcode_table[opcode];
}

void Decoder::decode_UD() {
  panic("Undefined instruction (%x)", opcode);
}

void Decoder::decode_wrapper() {
  operand_size = SIZE_32;
  opcode = fetcher.fetch<uint8_t>();
  current_decode_entry = &opcode_table[opcode];
  bool flag;
  do {
    flag = current_decode_entry->is_instr;
    if (current_decode_entry->default_operand_size)
      operand_size = current_decode_entry->default_operand_size;
    if (current_decode_entry->decode_helper) {
      (this->*(current_decode_entry->decode_helper))();
//    } else {
      // TODO: emit more hints
//      panic("Unexpected decode entry.");
    }
  } while (!flag);
}

