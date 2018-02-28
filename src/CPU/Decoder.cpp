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

inline void* Decoder::decode_op_I() {
  op_immd = fetcher.fetch_s(operand_size);
  return &op_immd;
}

inline void* Decoder::decode_op_Ib() {
  op_immd = (int8_t)fetcher.fetch<uint8_t>();
  return &op_immd;
}

inline void* Decoder::decode_op_a() {
  return &cpu.eax;
}

inline void* Decoder::decode_op_r() {
  return cpu.get_reg_ptr(opcode & 0x7, operand_size);
}

inline void* Decoder::decode_op_O() {
  uint32_t addr = fetcher.fetch<uint32_t>();
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
  dest = decode_op_a();
  src = decode_op_I();
}

void Decoder::decode_I_E2G() {
  decode_E2G();
  decode_op_I();
}

void Decoder::decode_I2E() {
  ModRM_decode();
  dest = ModRM_get_rm_ptr();
  src = decode_op_I();
}

void Decoder::decode_Ib2E() {
  ModRM_decode();
  dest = ModRM_get_rm_ptr();
  src = decode_op_Ib();
}

void Decoder::decode_I2r() {
  dest = decode_op_r();
  src = decode_op_I();
}

void Decoder::decode_O2a() {
  dest = decode_op_a();
  src = decode_op_O();
}

void Decoder::decode_a2O() {
  dest = decode_op_O();
  src = decode_op_a();
}

void Decoder::decode_IO_Ib() {
  dest = decode_op_a();
  op_immd = fetcher.fetch<uint8_t>();
}

void Decoder::decode_IO_dx() {
  dest = decode_op_a();
  op_immd = cpu.dx;
}

void Decoder::decode_I() {
  dest = decode_op_I();
}

void Decoder::decode_Ib() {
  dest = decode_op_Ib();
}

void Decoder::decode_r() {
  dest = decode_op_r();
}

void Decoder::decode_E() {
  ModRM_decode();
  dest = ModRM_get_rm_ptr();
}

void Decoder::decode_J() {
  op_immd = fetcher.fetch_s(operand_size);
  op_immd += fetcher.eip;
  dest = &op_immd;
}

void Decoder::decode_I_test() {
  src = decode_op_I();
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
#ifndef DEBUG
  throw std::runtime_error("Undefined instruction!");
#endif
  panic("Undefined instruction (%02x), eip = %08x", opcode, cpu.fetcher.eip);
}

void Decoder::decode_I2E_gp1() {
  decode_I2E();
  current_decode_entry = &groups_table[1][ModRM.reg_op];
}

void Decoder::decode_Ib2E_gp1() {
  decode_Ib2E();
  current_decode_entry = &groups_table[1][ModRM.reg_op];
}

void Decoder::decode_Ib2E_gp2() {
  decode_E();
  op_immd = fetcher.fetch<uint8_t>();
  src = &op_immd;
  current_decode_entry = &groups_table[2][ModRM.reg_op];
}

void Decoder::decode_one2E_gp2() {
  decode_E();
  op_immd = 1;
  src = &op_immd;
  current_decode_entry = &groups_table[2][ModRM.reg_op];
}

void Decoder::decode_cl2E_gp2() {
  decode_E();
  src = &cpu.cl;
  current_decode_entry = &groups_table[2][ModRM.reg_op];
}

void Decoder::decode_E_gp3() {
  decode_E();
  current_decode_entry = &groups_table[3][ModRM.reg_op];
}

void Decoder::decode_E_gp4() {
  decode_E();
  current_decode_entry = &groups_table[4][ModRM.reg_op];
}

void Decoder::decode_E_gp5() {
  decode_E();
  current_decode_entry = &groups_table[5][ModRM.reg_op];
}

void Decoder::decode_wrapper() {
  operand_size = SIZE_32;
  opcode = fetcher.fetch<uint8_t>();
  current_decode_entry = &opcode_table[opcode];
  bool flag;
  do {
    flag = current_decode_entry->is_instr;
    if (current_decode_entry->default_operand_size != SIZE_NONE)
      operand_size = current_decode_entry->default_operand_size;
    if (current_decode_entry->decode_helper) {
      (this->*(current_decode_entry->decode_helper))();
    }
  } while (!flag);
}

