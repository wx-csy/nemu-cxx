#include <SDL2/SDL.h>
#include <initializer_list>
#include <utility>
#include "common.h"
#include "devices/Keyboard.hpp"

#define _KEYS(_) \
  _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) \
  _(F11) _(F12) _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) \
  _(0) _(MINUS) _(EQUALS) _(BACKSPACE) _(TAB) _(Q) _(W) _(E) _(R) _(T) \
  _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
  _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) \
  _(APOSTROPHE) _(RETURN) _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) \
  _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) _(LCTRL) _(APPLICATION) _(LALT) \
  _(SPACE) _(RALT) _(RCTRL) _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) \
  _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)

#define _KEY_NAME(k) KEY_##k,

enum class KEYCODE : uint32_t {
  NONE = 0,
  _KEYS(_KEY_NAME)
};

#define _KEYMAP_ENTRY(k) {SDL_SCANCODE_##k,  KEYCODE::KEY_##k},

struct KEYMAP {
  KEYCODE lookup_table[256];
  KEYCODE operator [] (uint8_t index) {return lookup_table[index];}
  
  KEYMAP(std::initializer_list<std::pair<uint8_t, KEYCODE>> list) {
    for (auto item : list) lookup_table[item.first] = item.second;
  }

} keymap = {
  _KEYS(_KEYMAP_ENTRY)
};

static const uint8_t HASKEY_MASK = 0x1;
static const uint32_t KEYDOWN_MASK = 0x8000;

uint32_t Keyboard::read(pio_addr_t port, SIZE size) {
  if (port == DATA_PORT) {
    assert(size == SIZE_32);
    status &= ~HASKEY_MASK;
    return data;
  } else if (port == STATUS_PORT) {
    assert(size == SIZE_8);
    std::lock_guard<std::mutex> lock_queue(mutex_key_queue);
    if ((status & HASKEY_MASK) == 0 && !key_queue.empty()) {
      data = key_queue.front();
      status |= HASKEY_MASK;
      key_queue.pop();
    }
    return status;
  }
  panic("Keyboard does not support port 0x%x", port);
  return 0;
}

void Keyboard::write(pio_addr_t port, uint32_t data, SIZE size) {
  panic("Keyboard does not support port write!");
}

void Keyboard::update() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: exit(0);
      case SDL_KEYDOWN:
      case SDL_KEYUP: {
        if (event.key.repeat == 0) {
          uint8_t scancode = event.key.keysym.scancode;
          bool is_keydown = (event.key.type == SDL_KEYDOWN);
          if (keymap[scancode] != KEYCODE::NONE) {
            std::lock_guard<std::mutex> lock_queue(mutex_key_queue);
            key_queue.push((uint32_t)keymap[scancode] 
                | (is_keydown ? KEYDOWN_MASK : 0));
          }
        }
      }
    }
  }
}

