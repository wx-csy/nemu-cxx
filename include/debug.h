#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "cstdio"

#ifdef DEBUG

#include <cassert>

#define Assert(cond, ...) \
  do { \
    if (!(cond)) { \
      std::fflush(stdout); \
      std::fprintf(stderr, "\33[1;31m"); \
      std::fprintf(stderr, __VA_ARGS__); \
      std::fprintf(stderr, "\33[0m\n"); \
      assert(cond); \
    } \
  } while (0)

#define panic(format, ...) \
  Assert(0, format, ## __VA_ARGS__)

#else

#define NDEBUG
#include <cassert>

#define Assert(cond, ...)

#define panic(format, ...)

#endif

#endif

