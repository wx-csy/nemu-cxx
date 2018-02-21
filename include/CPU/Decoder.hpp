#ifndef __CPU_DECODER_HPP__
#define __CPU_DECODER_HPP__

#include <type_traits>
#include "common.h"
//#include "CPU.hpp"
#include "CPU/Fetcher.hpp"

struct CPU;
template <typename T> struct Executer;

struct Decoder { 
  CPU& cpu;
  Fetcher& fetcher;

  SIZE operand_size;
  void *src, *dest; 
  uint32_t op_immd;
  uint8_t opcode;
  
  Decoder(CPU& cpu);
  Decoder(CPU& cpu, Fetcher& fetcher);

  // ModRM information
  struct {
    uint8_t R_M    :3;
    uint8_t reg_op :3;
    uint8_t mod    :2;
  } ModRM;
  
  void ModRM_decode();
  void* ModRM_get_reg_ptr();
  vaddr_t ModRM_get_mem_vaddr();
  void* ModRM_get_rm_ptr();
  void* ModRM_get_rm_ptr(SIZE sz);

  struct decode_entry {
    void (Decoder::*decode_helper)();
    void (Executer<uint32_t>::*exec32_helper)();
    void (Executer<uint16_t>::*exec16_helper)();
    void (Executer<uint8_t>::*exec8_helper)();
    SIZE default_operand_size;
    bool is_instr;
  };

  static const decode_entry opcode_table[256];
  static const decode_entry twobyte_opcode_table[256];
  static const decode_entry groups_table[8][8];
  
  const decode_entry *current_decode_entry;
  
  void decode_wrapper();

  void* decode_op_I();
  void* decode_op_SI();
  void* decode_op_a();
  void* decode_op_r();
  void* decode_op_O();
 
  // Unary decoder
  void decode_I(); 
  void decode_r();
  void decode_E();
  void decode_I_test();
  void decode_J();

  // Binary decoder
  void decode_G2E();
  void decode_E2G();
  void decode_Eb2G();
  void decode_Ew2G();
  void decode_M2G_lea();
  void decode_I2a();
  void decode_I2E();
  void decode_SI2E();
  void decode_I2r();
  void decode_O2a();
  void decode_a2O(); 

  // Ternary decoder
  void decode_I_E2G(); // for imul, the imm must be accessed thru op_immd
  
  // Special
  void decode_prefix();
  void decode_UD();
  void decode_twobyte_escape();
  
  void decode_I2E_gp1();
  void decode_SI2E_gp1();
  void decode_Ib2E_gp2();
  void decode_group3();
  void decode_group4();
  void decode_E_gp5();
  void decode_group7();
  
};

#endif


