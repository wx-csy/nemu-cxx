#ifndef __DEVICES_VGA_HPP__
#define __DEVICES_VGA_HPP__

#include <SDL2/SDL.h>
#include "common.h"
#include "devices/mmio_device.hpp"

struct VGA : mmio_device {
  static const paddr_t VGA_PMEM_BASE = 0x40000;
  
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  VGA(Memory& memory);
  void read(paddr_t addr, uint32_t& data);
  void write(paddr_t addr, uint32_t data);
};

#endif
