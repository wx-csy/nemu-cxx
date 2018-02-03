#include "common.h"
#include "cpu.h"

struct ModRM_Decoder {
  struct {
    uint8_t R_M     :3;
    uint8_t reg_op  :3;
    uint8_t mod     :2;
  } modrm;
  
  Decoder& decoder;

  ModRM_Decoder(Decoder& decoder) : decoder(decoder) {
    reinterpret_cast<uint8_t&>(modrm) = 
      decoder.cpu.instr_fetch<uint8_t>();
    decoder.opcode_ext = modrm.reg_op;
  }
 
  void* get_reg_ptr() {
    return decoder.cpu.regs.get_reg_ptr(modrm.reg_op, 
        decoder.operand_size);
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
      reinterpret_cast<uint8_t&>(sib) = 
        decoder.cpu.instr_fetch<uint8_t>();
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
      disp = decoder.cpu.instr_vfetch_sext(disp_size);
      addr += disp;
    }
  
    if (base_reg != -1) 
      addr += decoder.cpu.regs.gpr[base_reg]._32;
  
    if (index_reg != -1) {
      addr += decoder.cpu.regs.gpr[index_reg]._32 << scale;
    }
  
    return addr;
  } 
  
  void* get_rm_ptr() {
    if (modrm.mod == 3) {
      return decoder.cpu.regs.get_reg_ptr(modrm.R_M, 
          decoder.operand_size);
    } else {
      return decoder.memory_access(get_mem_vaddr(), 
          decoder.operand_size); 
    }
  }
  
  void* get_rm_ptr(SIZE sz) {
    if (modrm.mod == 3) {
      return decoder.cpu.regs.get_reg_ptr(modrm.R_M, sz);
    } else {
      return decoder.memory_access(get_mem_vaddr(), sz);
    }
  }
};

Decoder::Decoder(CPU& cpu) : cpu(cpu) {
  operand_size = SIZE_32;
}

Decoder::~Decoder() {

}

inline void* Decoder::memory_access(vaddr_t addr, SIZE sz) {
  return cpu.mmu.memory.pmem + cpu.mmu.address_translate(addr);
}


inline void* Decoder::decop_immd() {
  op_immd = cpu.instr_vfetch(operand_size);
  return &op_immd;
}

inline void* Decoder::decop_simmd() {
  op_immd = cpu.instr_vfetch_sext(operand_size);
  return &op_immd;
}

inline void* Decoder::decop_regA() {
  return &cpu.regs.eax;
}

inline void* Decoder::decop_reg() {
  return cpu.regs.get_reg_ptr(opcode & 0x7, operand_size);
}

inline void* Decoder::decop_offset() {
  uint32_t addr = cpu.instr_vfetch_sext(operand_size);
  return memory_access(addr, operand_size);
}



// reg -> r/m
void Decoder::dec_G2E() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_rm_ptr();
  src = modrm_dec.get_reg_ptr();
}

// r/m -> reg
void Decoder::dec_E2G() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr();
  src = modrm_dec.get_rm_ptr();
}

// r/m (byte) -> reg
void Decoder::dec_Eb2G() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr();
  src = modrm_dec.get_rm_ptr(SIZE_8); 
}

void Decoder::dec_Ew2G() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr();
  src = modrm_dec.get_rm_ptr(SIZE_16);
}

// r/m (mem only) -> reg (for lea only)
void Decoder::dec_M2G_lea() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_reg_ptr(); 
  Assert(modrm_dec.modrm.mod != 3, "Invalid lea operand! (#UD)");
  op_immd = modrm_dec.get_mem_vaddr(); 
  src = &op_immd;
}

void Decoder::dec_I2a() {
  dest = decop_regA();
  src = decop_immd();
}

void Decoder::dec_I_E2G() {
  dec_E2G();
  decop_immd();
}

void Decoder::dec_I2E() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_rm_ptr();
  src = decop_immd();
}

void Decoder::dec_I2r() {
  dest = decop_reg();
  src = decop_immd();
}

void Decoder::dec_I() {
  dest = decop_immd();
}

void Decoder::dec_r() {
  dest = decop_reg();
}

void Decoder::dec_E() {
  ModRM_Decoder modrm_dec(*this);
  dest = modrm_dec.get_rm_ptr();
}

void Decoder::dec_I_test() {
  src = decop_immd();
}

