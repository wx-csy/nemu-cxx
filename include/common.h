#ifndef __COMMON_H__
#define __COMMON_H__

// #define DEBUG
#define LOG
#define HAS_MMIO
// #define HAS_TLB

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <iostream>

#include "debug.h"
#include "log.h"

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

enum SIZE : uint8_t {SIZE_NONE = 0, SIZE_8 = 1, SIZE_16 = 2, SIZE_32 = 4};

#endif

