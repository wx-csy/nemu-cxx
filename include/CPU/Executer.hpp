#ifndef __EXECUTER_HPP__
#define __EXECUTER_HPP__

#include <type_traits>
#include "common.h"
#include "CPU/CPU.hpp"
#include "CPU/Decoder.hpp"

template <typename T>
struct Executer {
  static_assert(std::is_unsigned<T>::value,
      "Template argument for `Executer' must be unsigned!");

  typedef typename std::make_signed<T>::type ST;

  CPU& cpu;
  T *const &src, *const &dest;
  const T* const immd;

  Executer(Decoder& decoder) :
    cpu(decoder.cpu),
    src(reinterpret_cast<T*&>(decoder.src)), 
    dest(reinterpret_cast<T*&>(decoder.dest)),
    immd(reinterpret_cast<T*>(&decoder.op_immd)) {}
  
  
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
private:
  void update_ZFSFPF(T x) {
    cpu.regs.eflags.ZF = (x == 0);
    cpu.regs.eflags.SF = (ST(x) < 0);
    cpu.regs.eflags.PF = __builtin_parity(uint8_t(x));
  }

  void clear_CFOF() {
    cpu.regs.eflags.CF = 0;
    cpu.regs.eflags.OF = 0;
  }

  void add_set_CF(T a, T b, T* c) {
    cpu.regs.eflags.CF = __builtin_add_overflow(a, b, c);
  }

  void add_set_OF(T a, T b, T* c) {
    cpu.regs.eflags.OF = 
      __builtin_add_overflow(a, b, reinterpret_cast<ST*>(c));
  }

  void sub_set_CF(T a, T b, T* c) {
    cpu.regs.eflags.CF = __builtin_sub_overflow(a, b, c);
  }

  void sub_set_OF(T a, T b, T* c) {
    cpu.regs.eflags.OF = 
      __builtin_sub_overflow(a, b, reinterpret_cast<ST*>(c));
  }

  void push(T x) {
    cpu.regs.esp -= 4;
    cpu.mmu.vaddr_write<uint32_t>(cpu.regs.esp, x);
  }

  T pop() {
    T temp = cpu.mmu.vaddr_read<uint32_t>(cpu.regs.esp);
    cpu.regs.esp += 4;
    return temp;
  }
};

#endif

