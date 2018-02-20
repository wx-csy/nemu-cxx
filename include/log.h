#ifndef __LOG_H__
#define __LOG_H__

#ifdef LOG
  extern FILE* log_fp;
  
  #define Log_write(format, ...) \
    do { \
      if (log_fp) { \
        fprintf(log_fp, format, ## __VA_ARGS__); \
        fflush(log_fp); \
      } \
    } while (0)

  #define Log(format, ...) \
    do { \
      fprintf(stdout, "\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
          __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
      fflush(stdout);\
      Log_write("[%s,%d,%s] " format "\n", \
          __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
    } while (0)

#else
  
  #define Log_write(format, ...) 

  #define Log(format, ...) 

#endif

#endif

