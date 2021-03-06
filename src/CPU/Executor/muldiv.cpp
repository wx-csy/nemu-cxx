#include "common.h"
#include "CPU.hpp"

template <typename T>
void Executor<T>::imul_set_CFOF(T a, T b, T* c) {
  cpu.eflags.CF = cpu.eflags.OF = 
    __builtin_mul_overflow(ST(a), ST(b), reinterpret_cast<ST*>(c));
}

template <>
void Executor<uint8_t>::MUL() {
  cpu.ax = (uint16_t)(*dest) * cpu.al;
  cpu.eflags.CF = cpu.eflags.OF = (bool)cpu.ah;
}

template <>
void Executor<uint16_t>::MUL() {
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
void Executor<uint32_t>::MUL() {
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
void Executor<uint8_t>::IMUL1() {
  cpu.ax = (int16_t)(int8_t)(*dest) * (int8_t)cpu.al;
  cpu.eflags.CF = cpu.eflags.OF = ((int8_t)cpu.ah == cpu.ah);
}

template <>
void Executor<uint16_t>::IMUL1() {
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
void Executor<uint32_t>::IMUL1() {
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
void Executor<T>::IMUL2() {
  imul_set_CFOF(*dest, *src, dest);
}

template <typename T>
void Executor<T>::IMUL3() {
  imul_set_CFOF(cpu.decoder.op_immd, *src, dest);
}

template <>
void Executor<uint8_t>::DIV() {
  uint8_t rem = cpu.ax % *dest;
  cpu.al = cpu.ax / *dest;
  cpu.ah = rem;
}

template <>
void Executor<uint16_t>::DIV() {
  union {
    struct {uint16_t low, high;};
    uint32_t value;
  } dividend;
  dividend.low = cpu.ax;
  dividend.high = cpu.dx;
  uint16_t rem = dividend.value % *dest;
  cpu.ax = dividend.value / *dest;
  cpu.dx = rem;
}

template <>
void Executor<uint32_t>::DIV() {
  union {
    struct {uint32_t low, high;};
    uint64_t value;
  } dividend;
  dividend.low = cpu.eax;
  dividend.high = cpu.edx;
  uint32_t rem = dividend.value % *dest;
  cpu.eax = dividend.value / *dest;
  cpu.edx = rem;
}

template <>
void Executor<uint8_t>::IDIV() {
  int8_t rem = (int16_t)cpu.ax % (int8_t)*dest;
  cpu.al = (int16_t)cpu.ax / (int8_t)*dest;
  cpu.ah = rem;
}

template <>
void Executor<uint16_t>::IDIV() {
  union {
    struct {uint16_t low, high;};
    int32_t value;
  } dividend;
  dividend.low = cpu.ax;
  dividend.high = cpu.dx;
  int16_t rem = dividend.value % (int16_t)*dest;
  cpu.ax = dividend.value / (int16_t)*dest;
  cpu.dx = rem;
}

template <>
void Executor<uint32_t>::IDIV() {
  union {
    struct {uint32_t low, high;};
    int64_t value;
  } dividend;
  dividend.low = cpu.eax;
  dividend.high = cpu.edx;
  int32_t rem = dividend.value % (int32_t)*dest;
  cpu.eax = dividend.value / (int32_t)*dest;
  cpu.edx = rem;
}

template class Executor<uint8_t>;
template class Executor<uint16_t>;
template class Executor<uint32_t>;

