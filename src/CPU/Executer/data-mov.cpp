#include "common.h"
#include "CPU/Executer.hpp"

template <typename T>
void Executer<T>::MOV() {
  *dest = *src;
}

template <typename T>
void Executer<T>::XCHG() {
  T temp = *dest;
  *dest = *src;
  *src = temp;
}

template <typename T>
void Executer<T>::PUSH() {
  push(*src);
}

template <typename T>
void Executer<T>::POP() {
  *dest = pop();
}

template <typename T>
void Executer<T>::LEAVE() {
  cpu.regs.esp = cpu.regs.ebp;
  cpu.regs.ebp = pop();
}

template <>
void Executer<uint32_t>::CLTD() {
  cpu.regs.edx = int32_t(cpu.regs.eax) >> 31;
}

template <>
void Executer<uint16_t>::CLTD() {
  cpu.regs.dx = int16_t(cpu.regs.ax) >> 15;
}

template <>
void Executer<uint32_t>::CWTL() {
  cpu.regs.eax = int16_t(cpu.regs.ax);
}

template <>
void Executer<uint16_t>::CWTL() {
  cpu.regs.ax = int8_t(cpu.regs.al);
}

template <typename T>
void Executer<T>::MOVSB() {
  *dest = reinterpret_cast<int8_t&>(*src);
}

template <typename T>
void Executer<T>::MOVSW() {
  *dest = reinterpret_cast<int16_t&>(*src);
}

template <typename T>
void Executer<T>::MOVZB() {
  *dest = reinterpret_cast<uint8_t&>(*src);
}

template <typename T>
void Executer<T>::MOVZW() {
  *dest = reinterpret_cast<uint16_t&>(*src);
}

template <typename T>
void Executer<T>::LEA() {
  *dest = *src;
}

template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

