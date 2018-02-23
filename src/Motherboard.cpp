#include <cstring>
#include "common.h"
#include "Memory.hpp"
#include "CPU.hpp"
#include "Debugger.hpp"
#include "devices/pio_device.hpp"
#include "devices/Serial.hpp"
#include "devices/Timer.hpp"
#include "devices/VGA.hpp"

namespace Motherboard {
  Memory memory;

#if HAS_MMIO
  VGA vga(memory);
#endif

  Ports ports;
  Serial serial(ports);
  Timer timer(ports);
  CPU cpu(memory, ports);
  
  Debugger debugger(cpu, memory);
};


FILE* log_fp;

int main(int argc, char* argv[]) {
  const char* img_path = NULL;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (strcmp(argv[i], "-b") == 0) 
        Log("This version of nemu does not have batch mode!");
      else if (strcmp(argv[i], "-l") == 0) {
        i++;
        Assert(i < argc, "Log file path missed!");
        log_fp = fopen(argv[i], "w");
        Assert(log_fp, "Can not open `%s'", argv[i]);
        Log("Log file is `%s'", argv[i]);
      } else {
        panic("Usage: %s [-b] [-l log_file] [img_file]", argv[0]);
      }
    } else {
      img_path = argv[i];
    }
  }
  printf("Welcome to NEMU!\n");
  Motherboard::debugger.load_image(img_path);
  Motherboard::debugger.mainloop();
  return 0;
}

