#ifndef __DECODER_HPP__
#define __DECODER_HPP__

#include "common.h"
#include "CPU/CPU.hpp"

struct Decoder {
  friend class ModRM_Decoder;

  Decoder(CPU& cpu);
  ~Decoder();
  
  CPU& cpu;
  SIZE operand_size;
  void *src, *dest; 
  uint32_t op_immd;
  uint8_t opcode, opcode_ext;

private:
  void* memory_access(vaddr_t addr, SIZE sz);

  void* decop_immd();
  void* decop_simmd();
  void* decop_regA();
  void* decop_reg();
  void* decop_offset();
   
  vaddr_t load_addr();

  // Unary decoder
  void dec_I(); 
  void dec_r();
  void dec_E();
  void dec_I_test();

  // Binary decoder
  void dec_G2E();
  void dec_E2G();
  void dec_Eb2G();
  void dec_Ew2G();
  void dec_M2G_lea();
  void dec_I2a();
  void dec_I2E();
  void dec_I2r();
 
  // Ternary decoder
  void dec_I_E2G(); // for imul, the imm must be accessed thru op_immd
};

template <typename T> 
struct Executer;

struct decode_entry {
  void (Decoder::*decode_helper)();
  void (Executer<uint8_t>::*exec_8_helper)();
  void (Executer<uint16_t>::*exec_16_helper)();
  void (Executer<uint32_t>::*exec_32_helper)();
};


#endif

