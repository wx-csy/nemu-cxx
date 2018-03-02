#include "common.h"
#include "CPU.hpp"

template <typename T>
inline void Executor<T>::update_ZFSFPF(T x) {
  cpu.eflags.ZF = (x == 0);
  cpu.eflags.SF = (ST(x) < 0);
  cpu.eflags.PF = __builtin_parity(uint8_t(x));
}

template <typename T>
inline void Executor<T>::clear_CFOF() {
  cpu.eflags.CF = 0;
  cpu.eflags.OF = 0;
}

template <typename T>
inline void Executor<T>::add_set_CF(T a, T b, T* c) {
  cpu.eflags.CF = __builtin_add_overflow(a, b, c);
}

template <typename T>
inline void Executor<T>::add_set_OF(T a, T b, T* c) {
  cpu.eflags.OF = 
    __builtin_add_overflow(ST(a), ST(b), reinterpret_cast<ST*>(c));
}

template <typename T>
inline void Executor<T>::sub_set_CF(T a, T b, T* c) {
  cpu.eflags.CF = __builtin_sub_overflow(a, b, c);
}

template <typename T>
inline void Executor<T>::sub_set_OF(T a, T b, T* c) {
  cpu.eflags.OF = 
    __builtin_sub_overflow(ST(a), ST(b), reinterpret_cast<ST*>(c));
}

template <typename T>
void Executor<T>::ADD() {
  T temp;
  add_set_OF(*dest, *src, &temp);
  add_set_CF(*dest, *src, dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executor<T>::ADC() {
  T temp;
  T destval = *dest, srcval = *src;
  add_set_OF(destval, srcval, &temp);
  if (!cpu.eflags.OF) add_set_OF(temp, cpu.eflags.CF, &temp);
  add_set_CF(destval, cpu.eflags.CF, &destval);
  if (!cpu.eflags.CF) add_set_CF(destval, srcval, &destval); else destval += srcval;
  *dest = destval;
  update_ZFSFPF(*dest);  
}

template <typename T>
void Executor<T>::SUB() {
  T temp;
  sub_set_OF(*dest, *src, &temp);
  sub_set_CF(*dest, *src, dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executor<T>::SBB() {
  T temp;
  T destval = *dest, srcval = *src;
  sub_set_OF(destval, srcval, &temp);
  if (!cpu.eflags.OF) sub_set_OF(temp, cpu.eflags.CF, &temp);
  sub_set_CF(destval, cpu.eflags.CF, &destval);
  if (!cpu.eflags.CF) sub_set_CF(destval, srcval, &destval); else destval -= srcval;
  *dest = destval;
  update_ZFSFPF(*dest);
}

template <typename T>
void Executor<T>::CMP() {
  T temp;
  sub_set_OF(*dest, *src, &temp);
  sub_set_CF(*dest, *src, &temp);
  update_ZFSFPF(temp);
}

template <typename T>
void Executor<T>::INC() {
  add_set_OF(*dest, T(1), dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executor<T>::DEC() {
  sub_set_OF(*dest, T(1), dest);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executor<T>::NEG() {
  sub_set_OF(T(0), *dest, dest);
  cpu.eflags.CF = (ST(*dest) <= 0);
  update_ZFSFPF(*dest);
}

template <typename T>
void Executor<T>::AND() {
  clear_CFOF();
  update_ZFSFPF(*dest &= *src);
}

template <typename T>
void Executor<T>::OR() {
  clear_CFOF();
  update_ZFSFPF(*dest |= *src);
}

template <typename T>
void Executor<T>::TEST() {
  clear_CFOF();
  update_ZFSFPF(*dest & *src);
}

template <typename T>
void Executor<T>::XOR() {
  clear_CFOF();
  update_ZFSFPF(*dest ^= *src);
}

template <typename T>
void Executor<T>::NOT() {
  *dest = ~*dest; 
}

template <typename T>
void Executor<T>::SHL() {
  // TODO: update CF and OF
  update_ZFSFPF(*dest <<= (*src & 31));
}

template <typename T>
void Executor<T>::SHR() {
  // TODO: update CF and OF
  update_ZFSFPF(*dest >>= (*src & 31));
}

template <typename T>
void Executor<T>::SAR() {
  // TODO: update CF and OF
  update_ZFSFPF(reinterpret_cast<ST&>(*dest) >>= (*src & 31));
}

template <typename T>
void Executor<T>::ROL() {
  // TODO: update CF and OF
  const T mask = (8 * sizeof(T) - 1);
  T temp = *src & mask;
  update_ZFSFPF(*dest = (*dest << temp) | (*dest >> (-temp & mask)));
}

template <typename T>
void Executor<T>::ROR() {
  // TODO: update CF and OF
  const T mask = (8 * sizeof(T) - 1);
  T temp = *src & mask;
  update_ZFSFPF(*dest = (*dest >> temp) | (*dest << (-temp & mask)));
}

template <typename T>
void Executor<T>::SETcc() {
  *dest = cpu.getcc(cpu.decoder.opcode & 0xf);
}

template class Executor<uint8_t>;
template class Executor<uint16_t>;
template class Executor<uint32_t>;

