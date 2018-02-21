#ifndef __CPU_EXECUTER_HPP__
#define __CPU_EXECUTER_HPP__

#include <type_traits>
#include "common.h"

struct CPU;

template <typename T>
struct Executer {
  static_assert(std::is_unsigned<T>::value,
      "Template argument for `Executer' must be unsigned!");

  typedef typename std::make_signed<T>::type ST;
    
  CPU& cpu;
  T *const &dest, *const &src;

  Executer(CPU& cpu);

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
  void SETcc();

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
  void Jcc();
  void CALL();
  void RET();
  
  // special instructions
  void NEMU_TRAP();
  void NOP();
  void IN();
  void OUT();

private:
  void update_ZFSFPF(T x);
  void clear_CFOF();
  void add_set_CF(T a, T b, T* c);
  void add_set_OF(T a, T b, T* c);
  void sub_set_CF(T a, T b, T* c);
  void sub_set_OF(T a, T b, T* c);

  void push(T x);
  T pop();

};

#endif

