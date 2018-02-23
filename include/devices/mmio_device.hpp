#ifndef __DEVICES_MMIO_DEVICE_HPP__
#define __DEVICES_MMIO_DEVICE_HPP__

#include "common.h"

struct Memory;
struct mmio_device {
  virtual void read(paddr_t addr, uint32_t& data) = 0; 
  virtual void write(paddr_t addr, uint32_t data) = 0;

protected:
  void register_physical_page(Memory& memory, uint32_t page_no);
};


#endif
