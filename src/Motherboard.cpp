#include <cstring>
#include <thread>
#include <chrono>
#include "common.h"
#include "Memory.hpp"
#include "CPU.hpp"
#include "Debugger.hpp"
#include "devices/pio_device.hpp"
#include "devices/Serial.hpp"
#include "devices/Clock.hpp"
#include "devices/VGA.hpp"
#include "devices/Keyboard.hpp"

namespace Motherboard {
  Memory memory;
  
  Ports ports;

#ifdef HAS_MMIO
  VGA vga(memory);
  Keyboard keyboard(ports);
#endif

  Serial serial(ports);
  Clock clock(ports);
  CPU cpu(memory, ports);
  
  Debugger debugger(cpu, memory);

  void mainloop() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
#ifdef HAS_MMIO
      vga.update();
#endif
      cpu.NO_INTR.clear();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
#if defined HAS_MMIO
      keyboard.update();
#endif
      cpu.NO_INTR.clear();
    }
  }
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
  
  std::thread th(&Debugger::mainloop, Motherboard::debugger);
  
  Motherboard::mainloop();
  return 0;
}

