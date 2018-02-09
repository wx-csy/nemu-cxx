#include "common.h"
#include "CPU.hpp"
#include <iostream>
template <typename T>
void CPU::Executer<T>::MOV() {
  *dest = *src;
}

template <typename T>
void CPU::Executer<T>::XCHG() {
  T temp = *dest;
  *dest = *src;
  *src = temp;
}

template <typename T>
void CPU::Executer<T>::PUSH() {
  push(*src);
}

template <typename T>
void CPU::Executer<T>::POP() {
  *dest = pop();
}

template <typename T>
void CPU::Executer<T>::LEAVE() {
  cpu.esp = cpu.ebp;
  cpu.ebp = pop();
}

template <>
void CPU::Executer<uint32_t>::CLTD() {
  cpu.edx = int32_t(cpu.eax) >> 31;
}

template <>
void CPU::Executer<uint16_t>::CLTD() {
  cpu.dx = int16_t(cpu.ax) >> 15;
}

template <>
void CPU::Executer<uint32_t>::CWTL() {
  cpu.eax = int16_t(cpu.ax);
}

template <>
void CPU::Executer<uint16_t>::CWTL() {
  cpu.ax = int8_t(cpu.al);
}

template <typename T>
void CPU::Executer<T>::MOVSB() {
  *dest = reinterpret_cast<int8_t&>(*src);
}

template <typename T>
void CPU::Executer<T>::MOVSW() {
  *dest = reinterpret_cast<int16_t&>(*src);
}

template <typename T>
void CPU::Executer<T>::MOVZB() {
  *dest = reinterpret_cast<uint8_t&>(*src);
}

template <typename T>
void CPU::Executer<T>::MOVZW() {
  *dest = reinterpret_cast<uint16_t&>(*src);
}

template <typename T>
void CPU::Executer<T>::LEA() {
  *dest = *src;
}

template class CPU::Executer<uint8_t>;
template class CPU::Executer<uint16_t>;
template class CPU::Executer<uint32_t>;

