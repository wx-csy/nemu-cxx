#ifndef __DEVICES_KEYBOARD_HPP__
#define __DEVICES_KEYBOARD_HPP__

#include <queue>
#include <mutex>
#include "common.h"
#include "devices/pio_device.hpp"


struct Keyboard : pio_device {
  static const pio_addr_t KEYBOARD_PORT = 0x60;
  static const pio_addr_t DATA_PORT = 0x60;
  static const pio_addr_t STATUS_PORT = 0x64;

  uint32_t data = 0;
  uint8_t status = 0;
  
  //    Note: here the queue may be replaced by a handwritten one
  // 
  std::mutex mutex_key_queue;
  std::queue<int> key_queue;
  
  Keyboard(Ports& ports) {
    for (pio_addr_t port = KEYBOARD_PORT; port <= KEYBOARD_PORT + 4;
        port++) 
      register_port(ports, port);
   
    Log("Keyboard initialized."); 
  }

  uint32_t read(pio_addr_t port, SIZE size);
  void write(pio_addr_t port, uint32_t data, SIZE size);
  void update();
};

#endif

