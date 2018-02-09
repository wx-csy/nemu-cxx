#include <cstring>
#include <iostream>
#include "common.h"
#include "Debugger.hpp"
#include "CPU.hpp"
#include "Memory.hpp"

const std::map<std::string, Debugger::cmd_entry> Debugger::cmd_table = {
    {"help", {"Display help information", &Debugger::cmd_help}},
    {"q", {"Exit NEMU", &Debugger::cmd_q}}, 
};

Debugger::Debugger(CPU& cpu, Memory& memory) : 
  cpu(cpu), memory(memory) {}

static const uint8_t default_image [] = {
  0xb8, 0x34, 0x12, 0x00, 0x00,       // 100000: movl  $0x1234, %eax
  0xb9, 0x27, 0x00, 0x10, 0x00,       // 100005: movl  $0x100027, %ecx
  0x89, 0x01,                         // 10000a: movl  $eax, (%ecx)
  0x66, 0xc7, 0x41, 0x04, 0x01, 0x00, // 10000c: movw  $0x1, 0x4(%ecx)
  0xbb, 0x02, 0x00, 0x00, 0x00,       // 100012: movl  $0x2, %ebx
  0x66, 0xc7, 0x84, 0x99, 0x00, 0xe0, // 100017: movw  $0x1, -0x2000(%ecx, %ebx, 4)
  0xff, 0xff, 0x01, 0x00,
  0xb8, 0x00, 0x00, 0x00, 0x00,       // 100021: movl  $0x0, %eax
  0xd6,                               // 100026: nemu_trap
};

void Debugger::load_default_image() {
  memcpy(&memory.pmem[CPU::ENTRY_START], default_image, 
      sizeof(default_image));
}

void Debugger::cmd_help(std::istringstream& args) {
  std::string arg;
  if (args >> arg) {
    auto entry = cmd_table.find(arg);
    if (entry == cmd_table.end()) 
      printf("Unknown command '%s'\n", arg.c_str());
    else 
      printf("%s - %s\n", entry->first.c_str(), 
          entry->second.description);
  } else {
    for (auto& entry : cmd_table) {
      printf("%s - %s\n", entry.first.c_str(), entry.second.description);
    }
  }
}

void Debugger::cmd_q(std::istringstream& args) {
  std::exit(0);
}

void Debugger::mainloop() {
  while (true) {
    std::cout << "(nemu) ";
    std::string str, cmd;
    std::getline(std::cin, str);
    std::istringstream args(str);
    if (args >> cmd) {
      auto entry = cmd_table.find(cmd);
      if (entry == cmd_table.end()) {
        printf("Unknown command '%s'\n", cmd.c_str());
      } else {
        (this->*entry->second.handler)(args);
      }
    } else {
      continue;
    }
  }
}

