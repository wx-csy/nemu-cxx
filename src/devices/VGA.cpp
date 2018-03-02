#include <sys/time.h>
#include <signal.h>
#include <SDL2/SDL.h>
#include <cstring>
#include "common.h"
#include "devices/VGA.hpp"

static const int SCREEN_H = 300, SCREEN_W = 400;
VGA::VGA(Memory& memory) : mem_base(memory.pmem + VGA_PMEM_BASE) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(SCREEN_W * 2, SCREEN_H * 2, 0, &window, &renderer);
  SDL_SetWindowTitle(window, "NEMU-cxx");
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);

  Log("VGA initialized!");
}

void VGA::update() {  
  SDL_UpdateTexture(texture, NULL, mem_base, SCREEN_W * 4);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

