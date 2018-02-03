#include "common.h"
#include "CPU/Executer.hpp"

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
  cpu.regs.eflags.CF = (ST(*dest) <= 0);
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
  *dest = cpu.regs.getcc(decoder.opcode & 0xf);
}
template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

