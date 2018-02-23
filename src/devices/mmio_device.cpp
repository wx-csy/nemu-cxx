#include "common.h"
#include "devices/mmio_device.hpp"
#include "Memory.hpp"

void mmio_device::register_physical_page(Memory& memory, uint32_t page_no) {
  assert(page_no < Memory::size / 4096);
  assert(memory.mmio_map[page_no] == NULL);
  memory.mmio_map[page_no] = this;
}

