#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <exception>
#include <string>

#ifdef DEBUG

  #define assert(cond) \
    do { \
      if (!(cond)) { \
    throw std::runtime_error("Assertion `" #cond "' failed.\n" \
        "  at: " + std::string(__func__) +  ", " + std::to_string(__LINE__) + " (" + __FILE__ + ")"); \
      } \
    } while (0)

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

  #define assert(cond) 

  #define Assert(cond, ...)

  #define panic(format, ...)

#endif

#endif

#define __DEBUG__ fprintf(stderr, "%s, line = %d\n", __func__, __LINE__)

