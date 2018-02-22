#ifndef __DEVICES_SERIAL_HPP__
#define __DEVICES_SERIAL_HPP__

#include "common.h"
#include "devices/pio_device.hpp"

struct Serial : pio_device {
  static const pio_addr_t SERIAL_PORT = 0x3F8;
  static const pio_addr_t CH_PORT = SERIAL_PORT;
  static const pio_addr_t LSR_PORT = SERIAL_PORT + 5;

  Serial(Ports& ports) : pio_device(ports, SERIAL_PORT, SERIAL_PORT + 7)
  { Log("Serial initialized."); }

  uint32_t read(pio_addr_t port, SIZE size) {
    if (port == LSR_PORT) {
      assert(size == SIZE_8);
      return 0x20;
    }
    panic("Unknown port: 0x%x (of Serial)", port);
  }

  void write(pio_addr_t port, uint32_t data, SIZE size) {
    assert(size == SIZE_8);
    if (port == CH_PORT) {
      putc(data, stdout);
      if (data == '\n') fflush(stdout);
    }
  }
};

#endif

