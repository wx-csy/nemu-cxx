#include "Debugger/regs.h"
#include "common.h"

std::map <std::string, uint16_t> regid = {
  {"eax", 0x0000}, {"ecx", 0x0001}, {"edx", 0x0002}, {"ebx", 0x0003},
  {"esp", 0x0004}, {"ebp", 0x0005}, {"esi", 0x0006}, {"edi", 0x0007},
  {"ax",  0x0100}, {"bx",  0x0101}, {"cx",  0x0102}, {"dx",  0x0103},
  {"sp",  0x0104}, {"bp",  0x0105}, {"si",  0x0106}, {"di",  0x0107},
  {"al",  0x0200}, {"bl",  0x0201}, {"cl",  0x0202}, {"dl",  0x0203},
  {"ah",  0x0204}, {"bh",  0x0205}, {"ch",  0x0206}, {"dh",  0x0207},

  {"eip", 0xff00}, {"efl", 0xff01},
};

