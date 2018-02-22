#ifndef __DEVICES_PIO_DEVICE_HPP__
#define __DEVICES_PIO_DEVICE_HPP__

#include "common.h"

typedef uint16_t pio_addr_t;
struct pio_device;
typedef pio_device* Ports[65536];

struct pio_device {
  virtual uint32_t read(pio_addr_t port, SIZE size) = 0; 
  virtual void write(pio_addr_t port, uint32_t data, SIZE size) = 0;

protected:
  void register_port(Ports& ports, pio_addr_t port) {
    Assert(ports[port] == NULL, "Port 0x%x has already been used!", port);
    ports[port] = this;
  }
};


#endif
