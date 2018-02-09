#include "common.h"
#include "CPU.hpp"

struct ModRM_Decoder {
  struct {
    uint8_t R_M     :3;
    uint8_t reg_op  :3;
    uint8_t mod     :2;
  } modrm;
  
  CPU& cpu;

  ModRM_Decoder(CPU& cpu) : cpu(cpu) {
    reinterpret_cast<uint8_t&>(modrm) = cpu.instr_fetch<uint8_t>();
    cpu.opcode_ext = modrm.reg_op;
  }
 
  void* get_reg_ptr() {
    return cpu.get_reg_ptr(modrm.reg_op, cpu.operand_size);
  }

  vaddr_t get_mem_vaddr() {
    assert(modrm.mod != 3);
  
    vaddr_t addr = 0;
    int32_t disp = 0;
    SIZE disp_size = SIZE_32;
    int base_reg = -1, index_reg = -1, scale = 0;
    
    if (modrm.R_M == 4) {
      struct {
        uint8_t base  :3;
        uint8_t index :3;
        uint8_t ss    :2;
      } sib;
      reinterpret_cast<uint8_t&>(sib) = cpu.instr_fetch<uint8_t>();
      base_reg = sib.base;
      scale = sib.ss;
      if (sib.index != 4) index_reg = sib.index;
    } else {
      base_reg = modrm.R_M;
    } 

    if (modrm.mod == 0) {
      if (base_reg == 5) base_reg = -1; else disp_size = SIZE_NONE;
    } else if (modrm.mod == 1) disp_size = SIZE_8;

    if (disp_size) {
      disp = cpu.instr_vfetch_sext(disp_size);
      addr += disp;
    }
  
    if (base_reg != -1) 
      addr += cpu.gpr[base_reg]._32;
  
    if (index_reg != -1) {
      addr += cpu.gpr[index_reg]._32 << scale;
    }
  
    return addr;
  } 
  
  void* get_rm_ptr() {
    if (modrm.mod == 3) {
      return cpu.get_reg_ptr(modrm.R_M, cpu.operand_size);
    } else {
      return cpu.memory_access(get_mem_vaddr(), 
          cpu.operand_size); 
    }
  }
  
  void* get_rm_ptr(SIZE sz) {
    if (modrm.mod == 3) {
      return cpu.get_reg_ptr(modrm.R_M, sz);
    } else {
      return cpu.memory_access(get_mem_vaddr(), sz);
    }
  }
};

inline void* CPU::decode_op_immd() {
  op_immd = instr_vfetch(operand_size);
  return &op_immd;
}

inline void* CPU::decode_op_simmd() {
  op_immd = instr_vfetch_sext(operand_size);
  return &op_immd;
}

inline void* CPU::decode_op_regA() {
  return &eax;
}

inline void* CPU::decode_op_reg() {
  return get_reg_ptr(opcode & 0x7, operand_size);
}

inline void* CPU::decode_op_offset() {
  uint32_t addr = instr_vfetch_sext(operand_size);
  return memory_access(addr, operand_size);
}


// reg -> r/m
void CPU::decode_G2E() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_rm_ptr();
  src = modrm_dec.get_reg_ptr();
}

// r/m -> reg
void CPU::decode_E2G() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr();
  src = modrm_dec.get_rm_ptr();
}

// r/m (byte) -> reg
void CPU::decode_Eb2G() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr();
  src = modrm_dec.get_rm_ptr(SIZE_8); 
}

void CPU::decode_Ew2G() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr();
  src = modrm_dec.get_rm_ptr(SIZE_16);
}

// r/m (mem only) -> reg (for lea only)
void CPU::decode_M2G_lea() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr(); 
  Assert(modrm_dec.modrm.mod != 3, "Invalid lea operand! (#UD)");
  op_immd = modrm_dec.get_mem_vaddr(); 
  src = &op_immd;
}

void CPU::decode_I2a() {
  dest = decode_op_regA();
  src = decode_op_immd();
}

void CPU::decode_I_E2G() {
  decode_E2G();
  decode_op_immd();
}

void CPU::decode_I2E() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_rm_ptr();
  src = decode_op_immd();
}

void CPU::decode_I2r() {
  dest = decode_op_reg();
  src = decode_op_immd();
}

void CPU::decode_O2a() {
  dest = decode_op_regA();
  src = decode_op_offset();
}

void CPU::decode_a2O() {
  dest = decode_op_offset();
  src = decode_op_regA();
}

void CPU::decode_I() {
  dest = decode_op_immd();
}

void CPU::decode_r() {
  dest = decode_op_reg();
}

void CPU::decode_E() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_rm_ptr();
}

void CPU::decode_I_test() {
  src = decode_op_immd();
}

void CPU::decode_twobyte_escape() {
  opcode = instr_fetch<uint8_t>();
  current_decode_entry = &twobyte_opcode_table[opcode];
}

void CPU::decode_prefix() {
  opcode = instr_fetch<uint8_t>();
  current_decode_entry = &opcode_table[opcode];
}

void CPU::decode_UD() {
  panic("Undefined instruction (%x)", opcode);
}

void CPU::decode_wrapper() {
  operand_size = SIZE_32;
  opcode = instr_fetch<uint8_t>();
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

