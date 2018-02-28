#ifndef __DEVICES_VGA_HPP__
#define __DEVICES_VGA_HPP__

#include "common.h"
#include "Memory.hpp"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

struct VGA {
  static const paddr_t VGA_PMEM_BASE = 0x40000;
  
  uint8_t *mem_base; 
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  VGA(Memory& memory);
  void update();
};

#endif
