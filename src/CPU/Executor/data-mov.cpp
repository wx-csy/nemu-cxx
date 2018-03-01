#include "common.h"
#include "CPU.hpp"
#include <iostream>
template <typename T>
void Executor<T>::MOV() {
  *dest = *src;
}

template <typename T>
void Executor<T>::XCHG() {
  T temp = *dest;
  *dest = *src;
  *src = temp;
}

template <typename T>
void Executor<T>::PUSH() {
  push(*dest);
}

template <typename T>
void Executor<T>::POP() {
  *dest = pop();
}

template <typename T>
void Executor<T>::LEAVE() {
  cpu.esp = cpu.ebp;
  cpu.ebp = pop();
}

template <>
void Executor<uint32_t>::CLTD() {
  cpu.edx = int32_t(cpu.eax) >> 31;
}

template <>
void Executor<uint16_t>::CLTD() {
  cpu.dx = int16_t(cpu.ax) >> 15;
}

template <>
void Executor<uint32_t>::CWTL() {
  cpu.eax = int16_t(cpu.ax);
}

template <>
void Executor<uint16_t>::CWTL() {
  cpu.ax = int8_t(cpu.al);
}

template <typename T>
void Executor<T>::MOVSB() {
  *dest = *reinterpret_cast<int8_t*>(src);
}

template <typename T>
void Executor<T>::MOVSW() {
  *dest = *reinterpret_cast<int16_t*>(src);
}

template <typename T>
void Executor<T>::MOVZB() {
  *dest = *reinterpret_cast<uint8_t*>(src);
}

template <typename T>
void Executor<T>::MOVZW() {
  *dest = *reinterpret_cast<uint16_t*>(src);
}

template <typename T>
void Executor<T>::LEA() {
  *dest = *src;
}

template <typename T>
void Executor<T>::PUSHA() {
  Assert(sizeof(T) == 4, "Unimplemented!");
  uint32_t temp = cpu.esp;
  push(cpu.eax);
  push(cpu.ecx);
  push(cpu.edx);
  push(cpu.ebx);
  push(temp);
  push(cpu.ebp);
  push(cpu.esi);
  push(cpu.edi);
}

template <typename T>
void Executor<T>::POPA() {
  Assert(sizeof(T) == 4, "Unimplemented");
  cpu.edi = pop();
  cpu.esi = pop();
  cpu.ebp = pop();
  pop();
  cpu.ebx = pop();
  cpu.edx = pop();
  cpu.ecx = pop();
  cpu.eax = pop();
}

template class Executor<uint8_t>;
template class Executor<uint16_t>;
template class Executor<uint32_t>;

