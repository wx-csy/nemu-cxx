#include "common.h"
#include "Memory.hpp"
#include "CPU.hpp"
#include "Debugger.hpp"

namespace Motherboard {
  Memory memory;
  CPU cpu(memory);
  Debugger debugger(cpu, memory);
};

int main() {
  Motherboard::debugger.mainloop();
  return 0;
}

