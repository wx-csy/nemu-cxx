#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <type_traits>
#include "common.h"
#include "Memory.hpp"
#include "CPU/MMU.hpp"
#include "CPU/Fetcher.hpp"

struct CPU {
  static const vaddr_t ENTRY_START = 0x100000;

  Memory& memory;
  CPU(Memory& memory); 

  // ---------- General Purpose Register Set ----------
  union {
    struct {
      union {uint32_t eax; uint16_t ax; struct {uint8_t al, ah;};};
      union {uint32_t ecx; uint16_t cx; struct {uint8_t cl, ch;};};
      union {uint32_t edx; uint16_t dx; struct {uint8_t dl, dh;};};
      union {uint32_t ebx; uint16_t bx; struct {uint8_t bl, bh;};};
      union {uint32_t esp; uint16_t sp;};
      union {uint32_t ebp; uint16_t bp;};
      union {uint32_t esi; uint16_t si;};
      union {uint32_t edi; uint16_t di;};
    };
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];
  };

  union {
    struct {
      uint32_t CF:1, _reserved_0:1, PF:1, _reserved_1:1, AF:1, 
               _reserved_2:1, ZF:1, SF:1, TF:1, IF:1, DF:1, OF:1, IOPL:2,
               NT:1, _reserved_3:1, RF:1, VM:1, _reserved_4:14;
    } eflags;
    uint32_t efl;
  };

  void* get_reg_ptr(unsigned id, SIZE sz) {
    Assert(id >= 0 && id < 8, "Register id out of range!");
    switch (sz) {
      case SIZE_8:  return &gpr[id >> 1]._8[id & 1];
      case SIZE_16: return &gpr[id]._16;
      case SIZE_32: return &gpr[id]._32;
      default:      panic("Unexpected register size!");
    }
  }

  bool getcc(uint8_t subcode) {
    enum {
      CC_O, CC_NO, CC_B, CC_NB, CC_E, CC_NE, CC_BE, CC_NBE,
      CC_S, CC_NS, CC_P, CC_NP, CC_L, CC_NL, CC_LE, CC_NLE
    };
    bool result;
    switch (subcode & (~1)) {
      case CC_O:  result = eflags.OF; break;
      case CC_B:  result = eflags.CF; break;
      case CC_E:  result = eflags.ZF; break;
      case CC_BE: result = eflags.CF || eflags.ZF; break;
      case CC_S:  result = eflags.SF; break;
      case CC_P:  result = eflags.PF; break;
      case CC_L:  result = eflags.SF != eflags.OF; break;
      case CC_LE: result = eflags.ZF || eflags.SF != eflags.OF; break;
      default:    panic("Unexpected conditional code!");
    }
    if (subcode & 1) result = !result;
    return result;
  }
  
  
  // ---------- Memory Management Unit ----------
  MMU mmu;
  
  Fetcher fetcher;

  // ---------- Decode Helpers ----------
  SIZE operand_size;
  void *src, *dest; 
  uint32_t op_immd;
  uint8_t opcode, opcode_ext;

  template <typename T> struct Executer;

  struct decode_entry {
    void (CPU::*decode_helper)();
    void (CPU::Executer<uint32_t>::*exec32_helper)();
    void (CPU::Executer<uint16_t>::*exec16_helper)();
    void (CPU::Executer<uint8_t>::*exec8_helper)();
    SIZE default_operand_size;
    bool is_instr;
  };

  static const decode_entry opcode_table[256];
  static const decode_entry twobyte_opcode_table[256];
  static const decode_entry groups_table[8][8];
  
  const decode_entry *current_decode_entry;
  
  void decode_wrapper();

  void* decode_op_immd();
  void* decode_op_simmd();
  void* decode_op_regA();
  void* decode_op_reg();
  void* decode_op_offset();
 
  // Unary decoder
  void decode_I(); 
  void decode_r();
  void decode_E();
  void decode_I_test();

  // Binary decoder
  void decode_G2E();
  void decode_E2G();
  void decode_Eb2G();
  void decode_Ew2G();
  void decode_M2G_lea();
  void decode_I2a();
  void decode_I2E();
  void decode_I2r();
  void decode_O2a();
  void decode_a2O(); 

  // Ternary decoder
  void decode_I_E2G(); // for imul, the imm must be accessed thru op_immd
  
  // Special
  void decode_prefix();
  void decode_UD();
  void decode_twobyte_escape();
  
  void decode_group1();
  void decode_group2();
  void decode_group3();
  void decode_group4();
  void decode_group5();
  void decode_group7();

  // ---------- Executer ----------
  template <typename T>
  struct Executer {
    static_assert(std::is_unsigned<T>::value,
        "Template argument for `Executer' must be unsigned!");

    typedef typename std::make_signed<T>::type ST;
    
    CPU& cpu;
    T *const &dest, *const &src;

    Executer(CPU& cpu) :
      cpu(cpu),
      dest(reinterpret_cast<T*&>(cpu.dest)),
      src(reinterpret_cast<T*&>(cpu.src)) {} 

    // ALU instructions
    void ADD();
    void ADC();
    void SUB();
    void SBB();
    void CMP();
    void INC();
    void DEC();
    void NEG();
    void AND();
    void OR();
    void TEST();
    void XOR();
    void NOT();
    void SHL();
    void SHR();
    void SAR();
    void ROL();
    void ROR();
    void SETCC();

    // data-mov instructions
    void MOV();
    void XCHG();
    void PUSH();
    void POP();
    void LEAVE();
    void CLTD(); // cwd(cwtd) and cdq(cltd)
    void CWTL(); // cbw(cbtw) and cwde(cwtl)
    void MOVSB(); // movsbw and movsbl
    void MOVSW(); // movsww and movswl
    void MOVZB(); // movzbw and movzbl
    void MOVZW(); // movsbw and movsbl
    void LEA();
  
    // control instructions
    void JMP();
    void JCC();
    void CALL();
    void RET();
    
    // special instructions
    void NEMU_TRAP();

  private:
    void update_ZFSFPF(T x);
    void clear_CFOF();
    void add_set_CF(T a, T b, T* c);
    void add_set_OF(T a, T b, T* c);
    void sub_set_CF(T a, T b, T* c);
    void sub_set_OF(T a, T b, T* c);

    void push(T x) {
      cpu.esp -= 4;
      cpu.mmu.vaddr_write<uint32_t>(cpu.esp, x);
    }

    T pop() {
      T temp = cpu.mmu.vaddr_read<uint32_t>(cpu.esp);
      cpu.esp += 4;
      return temp;
    }
  };
  
  Executer<uint32_t> exec32;
  Executer<uint16_t> exec16;
  Executer<uint8_t> exec8;  

  // ---------- Controller ----------
  uint32_t eip_before_exec;
  void exec_wrapper();
};

#endif


