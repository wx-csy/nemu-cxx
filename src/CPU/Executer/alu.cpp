#include "common.h"
#include "CPU.hpp"

template <typename T>
inline void Executer<T>::update_ZFSFPF(T x) {
  cpu.eflags.ZF = (x == 0);
  cpu.eflags.SF = (ST(x) < 0);
  cpu.eflags.PF = __builtin_parity(uint8_t(x));
}

template <typename T>
inline void Executer<T>::clear_CFOF() {
  cpu.eflags.CF = 0;
  cpu.eflags.OF = 0;
}

template <typename T>
inline void Executer<T>::add_set_CF(T a, T b, T* c) {
  cpu.eflags.CF = __builtin_add_overflow(a, b, c);
}

template <typename T>
inline void Executer<T>::add_set_OF(T a, T b, T* c) {
  cpu.eflags.OF = 
    __builtin_add_overflow(a, b, reinterpret_cast<ST*>(c));
}

template <typename T>
inline void Executer<T>::sub_set_CF(T a, T b, T* c) {
  cpu.eflags.CF = __builtin_sub_overflow(a, b, c);
}

template <typename T>
inline void Executer<T>::sub_set_OF(T a, T b, T* c) {
  cpu.eflags.OF = 
    __builtin_sub_overflow(a, b, reinterpret_cast<ST*>(c));
}

template <typename T>
void Executer<T>::ADD() {
  T temp;
  add_set_OF(*dest, *src, &temp);
  add_set_CF(*dest, *src, dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executer<T>::ADC() {
  Assert(0, "Unimplemented!");
}

template <typename T>
void Executer<T>::SUB() {
  T temp;
  sub_set_OF(*dest, *src, &temp);
  sub_set_CF(*dest, *src, dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executer<T>::SBB() {
  Assert(0, "Unimplemented!");
}

template <typename T>
void Executer<T>::CMP() {
  T temp;
  sub_set_OF(*dest, *src, &temp);
  sub_set_CF(*dest, *src, &temp);
  update_ZFSFPF(temp);
}

template <typename T>
void Executer<T>::INC() {
  add_set_OF(*dest, T(1), dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executer<T>::DEC() {
  sub_set_OF(*dest, T(1), dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executer<T>::NEG() {
  sub_set_OF(T(0), *dest, dest);
  cpu.eflags.CF = (ST(*dest) <= 0);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executer<T>::AND() {
  clear_CFOF();
  update_ZFSFPF(*dest &= *src);
}

template <typename T>
void Executer<T>::OR() {
  clear_CFOF();
  update_ZFSFPF(*dest |= *src);
}

template <typename T>
void Executer<T>::TEST() {
  clear_CFOF();
  update_ZFSFPF(*dest & *src);
}

template <typename T>
void Executer<T>::XOR() {
  clear_CFOF();
  update_ZFSFPF(*dest ^= *src);
}

template <typename T>
void Executer<T>::NOT() {
  *dest = ~*dest; 
}

template <typename T>
void Executer<T>::SHL() {
  // TODO: update CF and OF
  update_ZFSFPF(*dest <<= (*src & 31));
}

template <typename T>
void Executer<T>::SHR() {
  // TODO: update CF and OF
  update_ZFSFPF(*dest >>= (*src & 31));
}

template <typename T>
void Executer<T>::SAR() {
  // TODO: update CF and OF
  update_ZFSFPF(reinterpret_cast<ST&>(*dest) >>= (*src & 31));
}

template <typename T>
void Executer<T>::ROL() {
  // TODO: update CF and OF
  const T mask = (8 * sizeof(T) - 1);
  T temp = *src & mask;
  update_ZFSFPF(*dest = (*dest << temp) | (*dest >> (-temp & mask)));
}

template <typename T>
void Executer<T>::ROR() {
  // TODO: update CF and OF
  const T mask = (8 * sizeof(T) - 1);
  T temp = *src & mask;
  update_ZFSFPF(*dest = (*dest >> temp) | (*dest << (-temp & mask)));
}

template <typename T>
void Executer<T>::SETCC() {
  *dest = cpu.getcc(cpu.decoder.opcode & 0xf);
}

template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

