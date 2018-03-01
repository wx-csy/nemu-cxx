#include "common.h"
#include "CPU.hpp"

#define PREF(decode_helper) \
  {&Decoder::decode_##decode_helper, NULL, NULL, NULL, SIZE_NONE, false}

#define EMPTY {&Decoder::decode_UD, NULL, NULL, NULL, SIZE_NONE, false}

#define SZ(size) {&Decoder::decode_prefix, NULL, NULL, NULL, size, false}

#define I(decode_helper, instr_name) \
  {&Decoder::decode_##decode_helper, \
   &Executor<uint32_t>::instr_name, \
   &Executor<uint16_t>::instr_name, \
   &Executor<uint8_t>::instr_name, \
   SIZE_NONE, true} 

#define IB(decode_helper, instr_name) \
  {&Decoder::decode_##decode_helper, \
   NULL, \
   NULL, \
   &Executor<uint8_t>::instr_name, \
   SIZE_8, true} 

#define IE(instr_name) \
  {NULL, \
   &Executor<uint32_t>::instr_name, \
   &Executor<uint16_t>::instr_name, \
   &Executor<uint8_t>::instr_name, \
   SIZE_NONE, true}

#define IvE(instr_name) \
  {NULL, \
   &Executor<uint32_t>::instr_name, \
   &Executor<uint16_t>::instr_name, \
   NULL, \
   SIZE_NONE, true}

#define G(group_decode_helper) \
  {&Decoder::decode_##group_decode_helper, \
    NULL, NULL, NULL, SIZE_NONE, false}

#define GB(group_decode_helper) \
  {&Decoder::decode_##group_decode_helper, \
    NULL, NULL, NULL, SIZE_8, false}

const Decoder::decode_entry Decoder::opcode_table[256] = {
  /* 0x00 */ IB(G2E, ADD), I(G2E, ADD), IB(E2G, ADD), I(E2G, ADD),
  /* 0x04 */ IB(I2a, ADD), I(I2a, ADD), EMPTY, EMPTY,
  /* 0x08 */ IB(G2E, OR), I(G2E, OR), IB(E2G, OR), I(E2G, OR),
  /* 0x0c */ IB(I2a, OR), I(I2a, OR), EMPTY, PREF(twobyte_escape),
  
  /* 0x10 */ IB(G2E, ADC), I(G2E, ADC), IB(E2G, ADC), I(E2G, ADC),
  /* 0x14 */ IB(I2a, ADC), I(I2a, ADC), EMPTY, EMPTY,
  /* 0x18 */ IB(G2E, SBB), I(G2E, SBB), IB(E2G, SBB), I(E2G, SBB),
  /* 0x1c */ IB(I2a, SBB), I(I2a, SBB), EMPTY, EMPTY,

  /* 0x20 */ IB(G2E, AND), I(G2E, AND), IB(E2G, AND), I(E2G, AND),
  /* 0x24 */ IB(I2a, AND), I(I2a, AND), EMPTY, EMPTY,
  /* 0x28 */ IB(G2E, SUB), I(G2E, SUB), IB(E2G, SUB), I(E2G, SUB),
  /* 0x2c */ IB(I2a, SUB), I(I2a, SUB), EMPTY, EMPTY,

  /* 0x30 */ IB(G2E, XOR), I(G2E, XOR), IB(E2G, XOR), I(E2G, XOR),
  /* 0x34 */ IB(I2a, XOR), I(I2a, XOR), EMPTY, EMPTY,
  /* 0x38 */ IB(G2E, CMP), I(G2E, CMP), IB(E2G, CMP), I(E2G, CMP),
  /* 0x3c */ IB(I2a, CMP), I(I2a, CMP), EMPTY, EMPTY,

  /* 0x40 */ I(r, INC), I(r, INC), I(r, INC), I(r, INC),
  /* 0x44 */ I(r, INC), I(r, INC), I(r, INC), I(r, INC), 
  /* 0x48 */ I(r, DEC), I(r, DEC), I(r, DEC), I(r, DEC), 
  /* 0x4c */ I(r, DEC), I(r, DEC), I(r, DEC), I(r, DEC), 

  /* 0x50 */ I(r, PUSH), I(r, PUSH), I(r, PUSH), I(r, PUSH),
  /* 0x54 */ I(r, PUSH), I(r, PUSH), I(r, PUSH), I(r, PUSH),
  /* 0x58 */ I(r, POP), I(r, POP), I(r, POP), I(r, POP),
  /* 0x5c */ I(r, POP), I(r, POP), I(r, POP), I(r, POP),

  /* 0x60 */ IE(PUSHA), IE(POPA), EMPTY, EMPTY,
  /* 0x64 */ EMPTY, EMPTY, SZ(SIZE_16), EMPTY,
  /* 0x68 */ I(I, PUSH), EMPTY, I(Ib, PUSH), EMPTY,
  /* 0x6c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x70 */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),
  /* 0x74 */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),
  /* 0x78 */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),
  /* 0x7c */ IB(J, Jcc), IB(J, Jcc), IB(J, Jcc), IB(J, Jcc),

  /* 0x80 */ GB(I2E_gp1), G(I2E_gp1), EMPTY, G(Ib2E_gp1),
  /* 0x84 */ IB(E2G, TEST), I(E2G, TEST), EMPTY, EMPTY,
  /* 0x88 */ IB(G2E, MOV), I(G2E, MOV), IB(E2G, MOV), I(E2G, MOV),
  /* 0x8c */ EMPTY, I(M2G_lea, LEA), EMPTY, EMPTY,

  /* 0x90 */ IE(NOP), EMPTY, EMPTY, EMPTY,
  /* 0x94 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x98 */ IvE(CWTL), IvE(CLTD), EMPTY, EMPTY,
  /* 0x9c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xa0 */ IB(O2a, MOV), I(O2a, MOV), IB(a2O, MOV), I(a2O, MOV),
  /* 0xa4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */ IB(I2a, TEST), I(I2a, TEST), EMPTY, EMPTY,
  /* 0xac */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xb0 */ IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV),
  /* 0xb4 */ IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV), IB(I2r, MOV),
  /* 0xb8 */ I(I2r, MOV), I(I2r, MOV), I(I2r, MOV), I(I2r, MOV),
  /* 0xbc */ I(I2r, MOV), I(I2r, MOV), I(I2r, MOV), I(I2r, MOV),

  /* 0xc0 */ GB(Ib2E_gp2), G(Ib2E_gp2), EMPTY, IE(RET),
  /* 0xc4 */ EMPTY, EMPTY, IB(I2E, MOV), I(I2E, MOV),
  /* 0xc8 */ EMPTY, IE(LEAVE), EMPTY, EMPTY,
  /* 0xcc */ EMPTY, IB(I, INT), EMPTY, IE(IRET),
  
  /* 0xd0 */ GB(one2E_gp2), G(one2E_gp2), GB(cl2E_gp2), G(cl2E_gp2),
  /* 0xd4 */ EMPTY, EMPTY, IE(NEMU_TRAP), EMPTY,
  /* 0xd8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0xe0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe8 */ I(J, CALL), I(J, JMP), EMPTY, IB(J, JMP),
  /* 0xec */ IB(IO_dx, IN), I(IO_dx, IN), IB(IO_dx, OUT), I(IO_dx, OUT), 

  /* 0xf0 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */ EMPTY, EMPTY, GB(E_gp3), G(E_gp3),
  /* 0xf8 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */ EMPTY, EMPTY, GB(E_gp4), G(E_gp5),
};

const Decoder::decode_entry Decoder::twobyte_opcode_table[256] = {
  /* 0x00 */ EMPTY, G(M_gp7), EMPTY, EMPTY,
  /* 0x04 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x08 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x0c */ EMPTY, EMPTY, EMPTY, EMPTY,
  
  /* 0x10 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x14 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x1c */ EMPTY, EMPTY, EMPTY, EMPTY,

  /* 0x20 */ I(G2E, MOV_cr2r), EMPTY, I(E2G, MOV_r2cr), EMPTY,
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
  /* 0xac */ EMPTY, EMPTY, EMPTY, I(E2G, IMUL2),

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
  /* 0x10 */ IE(ADD), IE(OR), IE(ADC), IE(SBB),
  /* 0x14 */ IE(AND), IE(SUB), IE(XOR), IE(CMP),
},
{ // GROUP 2
  /* 0x20 */ IE(ROL), IE(ROR), EMPTY, EMPTY,
  /* 0x24 */ IE(SHL), IE(SHR), EMPTY, IE(SAR),
},
{ // GROUP 3
  /* 0x30 */ I(I_test, TEST), EMPTY, IE(NOT), IE(NEG),
  /* 0x34 */ IE(MUL), IE(IMUL1), IE(DIV), IE(IDIV),
},
{ // GROUP 4
  /* 0x40 */ IE(INC), IE(DEC), EMPTY, EMPTY,
  /* 0x44 */ EMPTY, EMPTY, EMPTY, EMPTY,
},
{ // GROUP 5
  /* 0x50 */ IE(INC), IE(DEC), IE(CALL), EMPTY,
  /* 0x54 */ IE(JMP), EMPTY, IE(PUSH), EMPTY,
},
{ // GROUP 6
  /* 0x60 */ EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */ EMPTY, EMPTY, EMPTY, EMPTY,
},
{ // GROUP 7
  /* 0x70 */ EMPTY, EMPTY, EMPTY, IE(LIDT),
  /* 0x74 */ EMPTY, EMPTY, EMPTY, EMPTY,
}
};

