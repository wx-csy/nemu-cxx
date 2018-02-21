#include "common.h"
#include "CPU.hpp"

#define PREF(decode_helper) \
  {&Decoder::decode_##decode_helper, NULL, NULL, NULL, SIZE_NONE, false}

#define EMPTY {&Decoder::decode_UD, NULL, NULL, NULL, SIZE_NONE, false}

#define SZ(size) {&Decoder::decode_prefix, NULL, NULL, NULL, size, false}

#define I(decode_helper, instr_name) \
  {&Decoder::decode_##decode_helper, \
   &Executer<uint32_t>::instr_name, \
   &Executer<uint16_t>::instr_name, \
   &Executer<uint8_t>::instr_name, \
   SIZE_NONE, true} 

#define IB(decode_helper, instr_name) \
  {&Decoder::decode_##decode_helper, \
   &Executer<uint32_t>::instr_name, \
   &Executer<uint16_t>::instr_name, \
   &Executer<uint8_t>::instr_name, \
   SIZE_8, true} 

#define IE(instr_name) \
  {NULL, \
   &Executer<uint32_t>::instr_name, \
   &Executer<uint16_t>::instr_name, \
   &Executer<uint8_t>::instr_name, \
   SIZE_NONE, true} 

#define G(group_decode_helper) \
  {&Decoder::decode_##group_decode_helper, \
    NULL, NULL, NULL, SIZE_NONE, false}

#define GB(group_decode_helper) \
  {&Decoder::decode_##group_decode_helper, \
    NULL, NULL, NULL, SIZE_8, false}

const Decoder::decode_entry Decoder::opcode_table[256] = {
  /* 0x00 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x04 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x08 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x0c */ EMPTY, EMPTY, EMPTY, PREF(twobyte_escape),
  
  /* 0x10 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x14 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x1c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x20 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x24 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x28 */ IB(G2E, SUB), I(G2E, SUB), IB(E2G, SUB), I(E2G, SUB),
  /* 0x2c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x30 */ IB(G2E, XOR), I(G2E, XOR), IB(E2G, XOR), I(E2G, XOR),
  /* 0x34 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x38 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x3c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x40 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x44 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x48 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x4c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x50 */ I(r, PUSH), I(r, PUSH), I(r, PUSH), I(r, PUSH),
  /* 0x54 */ I(r, PUSH), I(r, PUSH), I(r, PUSH), I(r, PUSH),
  /* 0x58 */ I(r, POP), I(r, POP), I(r, POP), I(r, POP),
  /* 0x5c */ I(r, POP), I(r, POP), I(r, POP), I(r, POP),

  /* 0x60 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */ EMPTY, EMPTY, SZ(SIZE_16), EMPTY,
  /* 0x68 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x6c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x70 */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),
  /* 0x74 */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),
  /* 0x78 */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),
  /* 0x7c */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),

  /* 0x80 */ GB(I2E_gp1), G(I2E_gp1), EMPTY, G(SI2E_gp1),
  /* 0x84 */ IB(E2G, TEST), I(E2G, TEST), EMPTY, EMPTY,
  /* 0x88 */ IB(G2E, MOV), I(G2E, MOV), IB(E2G, MOV), I(E2G, MOV),
  /* 0x8c */ EMPTY, I(M2G_lea, LEA), EMPTY, EMPTY,

  /* 0x90 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x94 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x98 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x9c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xa0 */ IB(O2a, MOV), I(O2a, MOV), IB(a2O, MOV), I(a2O, MOV),
  /* 0xa4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xac */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xb0 */ IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV),
  /* 0xb4 */ IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV),
  /* 0xb8 */ I(I2r, MOV), I(I2r, MOV), I(I2r, MOV), I(I2r, MOV),
  /* 0xbc */ I(I2r, MOV), I(I2r, MOV), I(I2r, MOV), I(I2r, MOV),

  /* 0xc0 */ EMPTY, EMPTY, EMPTY, IE(RET),
  /* 0xc4 */ EMPTY, EMPTY, IB(I2E, MOV), I(I2E, MOV),
  /* 0xc8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xcc */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xd0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd4 */ EMPTY, EMPTY, IE(NEMU_TRAP), EMPTY,
  /* 0xd8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xe0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe8 */ I(J, CALL), I(J, JMP), EMPTY, IB(J, JMP),
  /* 0xec */ EMPTY, EMPTY, IE(OUT), EMPTY, 

  /* 0xf0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */ EMPTY, EMPTY, EMPTY, G(E_gp5),
};

const Decoder::decode_entry Decoder::twobyte_opcode_table[256] = {
  /* 0x00 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x04 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x08 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x0c */ EMPTY, EMPTY, EMPTY, EMPTY,
  
  /* 0x10 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x14 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x1c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x20 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x24 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x28 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x2c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x30 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x34 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x38 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x3c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x40 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x44 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x48 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x4c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x50 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x54 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x58 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x5c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x60 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x68 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x6c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x70 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x74 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x78 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x7c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x80 */ I(J, Jcc), I(J, Jcc), I(J, Jcc), I(J, Jcc),
  /* 0x84 */ I(J, Jcc), I(J, Jcc), I(J, Jcc), I(J, Jcc),
  /* 0x88 */ I(J, Jcc), I(J, Jcc), I(J, Jcc), I(J, Jcc),
  /* 0x8c */ I(J, Jcc), I(J, Jcc), I(J, Jcc), I(J, Jcc),

  /* 0x90 */ IB(E, SETcc), IB(E, SETcc), IB(E, SETcc), IB(E, SETcc),
  /* 0x94 */ IB(E, SETcc), IB(E, SETcc), IB(E, SETcc), IB(E, SETcc),
  /* 0x98 */ IB(E, SETcc), IB(E, SETcc), IB(E, SETcc), IB(E, SETcc),
  /* 0x9c */ IB(E, SETcc), IB(E, SETcc), IB(E, SETcc), IB(E, SETcc),

  /* 0xa0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xac */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xb0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xb4 */ EMPTY, EMPTY, I(E2G, MOVZB), I(E2G, MOVZW),
  /* 0xb8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xbc */ EMPTY, EMPTY, I(E2G, MOVSB), I(E2G, MOVSW),

  /* 0xc0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xcc */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xd0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xe0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xec */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xf0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */ EMPTY, EMPTY, EMPTY, EMPTY,
};

const Decoder::decode_entry Decoder::groups_table[8][8] = {
{ // GROUP 0
  /* 0x00 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x04 */ EMPTY, EMPTY, EMPTY, EMPTY,
},
{ // GROUP 1
  /* 0x10 */ IE(ADD), IE(OR), EMPTY, EMPTY,
  /* 0x14 */ IE(AND), IE(SUB), IE(XOR), IE(CMP),
},
{ // GROUP 2
  /* 0x20 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x24 */ EMPTY, EMPTY, EMPTY, EMPTY,
},
{ // GROUP 3
  /* 0x30 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x34 */ EMPTY, EMPTY, EMPTY, EMPTY,
},
{ // GROUP 4
  /* 0x40 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x44 */ EMPTY, EMPTY, EMPTY, EMPTY,
},
{ // GROUP 5
  /* 0x50 */ IE(INC), IE(DEC), EMPTY, EMPTY,
  /* 0x54 */ EMPTY, EMPTY, IE(PUSH), EMPTY,
},
{ // GROUP 6
  /* 0x60 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */ EMPTY, EMPTY, EMPTY, EMPTY,
},
{ // GROUP 7
  /* 0x70 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x74 */ EMPTY, EMPTY, EMPTY, EMPTY,
}
};

