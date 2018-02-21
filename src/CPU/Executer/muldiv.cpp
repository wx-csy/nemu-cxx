#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executer<T>::imul_set_CFOF(T a, T b, T* c) {
  cpu.eflags.CF = cpu.eflags.OF = 
    __builtin_mul_overflow(ST(a), ST(b), reinterpret_cast<ST*>(c));
}

template <>
void Executer<uint8_t>::MUL() {
  cpu.ax = (uint16_t)(*dest) * cpu.al;
  cpu.eflags.CF = cpu.eflags.OF = (bool)cpu.ah;
}

template <>
void Executer<uint16_t>::MUL() {
  union {
    struct {uint16_t low, high;};
    uint32_t result;
  } product;
  product.result = (uint32_t)(*dest) * cpu.ax;
  cpu.ax = product.low;
  cpu.dx = product.high;
  cpu.eflags.CF = cpu.eflags.OF = (bool)cpu.dx;
}

template <>
void Executer<uint32_t>::MUL() {
  union {
    struct {uint32_t low, high;};
    uint64_t result;
  } product;
  product.result = (uint64_t)(*dest) * cpu.eax;
  cpu.eax = product.low;
  cpu.edx = product.high;
  cpu.eflags.CF = cpu.eflags.OF = (bool)cpu.edx;
}

template <>
void Executer<uint8_t>::IMUL1() {
  cpu.ax = (int16_t)(int8_t)(*dest) * (int8_t)cpu.al;
  cpu.eflags.CF = cpu.eflags.OF = ((int8_t)cpu.ah == cpu.ah);
}

template <>
void Executer<uint16_t>::IMUL1() {
  union {
    struct {uint16_t low, high;};
    uint32_t result;
  } product;
  product.result = (int32_t)(int16_t)(*dest) * (int16_t)cpu.ax;
  cpu.ax = product.low;
  cpu.dx = product.high;
  cpu.eflags.CF = cpu.eflags.OF = 
    ((int32_t)product.result == (int16_t)product.low);
}

template <>
void Executer<uint32_t>::IMUL1() {
  union {
    struct {uint32_t low, high;};
    uint64_t result;
  } product;
  product.result = (int64_t)(int32_t)(*dest) * (int32_t)cpu.eax;
  cpu.eax = product.low;
  cpu.edx = product.high;
  cpu.eflags.CF = cpu.eflags.OF = 
    ((int64_t)product.result == (int32_t)product.low);
}

template <typename T>
void Executer<T>::IMUL2() {
  imul_set_CFOF(*dest, *src, dest);
}

template <typename T>
void Executer<T>::IMUL3() {
  imul_set_CFOF(cpu.decoder.op_immd, *src, dest);
}

template class Executer<uint8_t>;
template class Executer<uint16_t>;
template class Executer<uint32_t>;

