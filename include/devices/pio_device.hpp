#ifndef __DEVICES_PIO_DEVICE_HPP__
#define __DEVICES_PIO_DEVICE_HPP__

#include "common.h"

typedef uint32_t pio_addr_t;
struct pio_device;
typedef pio_device* Ports[65536];

struct pio_device {
  pio_device(Ports& ports, pio_addr_t start, pio_addr_t end) {
    for (pio_addr_t port = start; port <= end; port++) {
      Assert(ports[port] == NULL, "Port 0x%x has already been occupied!",
          port);
      ports[port] = this;
    } 
  }

  virtual uint32_t read(pio_addr_t port, SIZE size) = 0; 
  virtual void write(pio_addr_t port, uint32_t data, SIZE size) = 0;
};


#endif
