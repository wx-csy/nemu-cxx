#include <cstring>
#include <iostream>
#include "common.h"
#include "Debugger.hpp"
#include "Debugger/expr.h"
#include "CPU.hpp"
#include "Memory.hpp"

const std::map<std::string, Debugger::cmd_entry> Debugger::cmd_table = {
  {"help", {"Display help information", &Debugger::cmd_help}},
  {"q", {"Exit NEMU", &Debugger::cmd_q}}, 
  {"c", {"Continue executing the program", &Debugger::cmd_c}},
  {"si", {"Execute one or more instructions", &Debugger::cmd_si}}, 
  {"p", {"Evaluate and print expression", &Debugger::cmd_p}},
  {"b", {"Add breakpoint", &Debugger::cmd_b}},
  {"i", {"Print CPU information", &Debugger::cmd_i}},
};

Debugger::Debugger(CPU& cpu, Memory& memory) : 
  cpu(cpu), memory(memory)
{ 
}

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

void Debugger::load_image(const char* path) {
  if (path == NULL) load_default_image();
  else {
    FILE *fp = fopen(path, "rb");
    Assert(fp, "Can not open image `%s'", path);
    Log("Image `%s' successfully loaded!", path);
    fseek(fp, 0, SEEK_END);
    uint32_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int ret = fread(&memory.pmem[CPU::ENTRY_START], size, 1, fp);
    assert(ret == 1);
    fclose(fp);
  }
}

void Debugger::load_default_image() {
  Log("Default image successfully loaded!");
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

void Debugger::cmd_c(std::istringstream& args) {
  while (true) {
    exec_wrapper();
  }
}

void Debugger::cmd_si(std::istringstream& args) {
  uint64_t num_of_instrs;
  if (!(args >> num_of_instrs)) {
    exec_wrapper();
 } else
  while (num_of_instrs--) exec_wrapper();
}

void Debugger::cmd_p(std::istringstream& args) {
  std::string expstr;
  getline(args, expstr);
  std::vector<token> tokens = tokenize(expstr.c_str());
  expr expression = build_expr(tokens.begin(), tokens.end());
  uint32_t value = expression();
  printf("  DEC:%d\tHEX: 0x%08x\n", value, value);
  // std::cout << expression() << std::endl;
}

void Debugger::cmd_b(std::istringstream& args) {
  std::string expstr;
  getline(args, expstr);
  std::vector<token> tokens = tokenize(expstr.c_str());
  expr expression = build_expr(tokens.begin(), tokens.end());
  uint32_t value = expression();
  breakpoints.insert(value);
  printf("  Breakpoint 0x%08x successfully added!\n", value);
}

void Debugger::cmd_i(std::istringstream& args) {
  int num_of_entries = 12;
  for (int i=0; i<num_of_entries; i++) {
    if (i < 8) printf("$gpr[%d]\t0x%08x\n", i, cpu.gpr[i]._32);
    else if (i == 9) printf("$eip\t0x%08x\n", cpu.fetcher.eip);
    else if (i == 10) printf("$efl\t0x%08x\n", cpu.efl);
  }
}

void Debugger::exec_wrapper() {
  cpu.exec_wrapper();

  if (breakpoints.find(cpu.fetcher.eip) != breakpoints.end())
    throw std::runtime_error("Breakpoint triggered!");
};

void Debugger::mainloop() {
  while (true) {
    try {
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
    } catch (std::exception& ex) {
      printf("Exception caught: %s\n", ex.what());
    }
  }
}

