#ifndef __DEVICES_TIMER_HPP__
#define __DEVICES_TIMER_HPP__

#include <ctime>
#include <limits>
#include "common.h"
#include "devices/pio_device.hpp"

struct Timer : pio_device {
  static const pio_addr_t RTC_PORT = 0x48;

  Timer(Ports& ports) : pio_device(ports, RTC_PORT, RTC_PORT + 3) {
    if (CLOCKS_PER_SEC < 1000)
      Log("\33[31mWarning: the timer of this platform has low accuracy. This may lead to loss of precision.\33[0m");
    
    if (std::numeric_limits<clock_t>::max() / (CLOCKS_PER_SEC / 1000ull)
        < 100000)
      Log("\33[31mWarning: the clock_t type in this platform is too short. This may lead to incorrect behavior of timer.\33[0m");
    
    Log("Timer initialized.");
  }

  uint32_t read(pio_addr_t port, SIZE size) {
    assert(size == SIZE_32 && port == RTC_PORT);
    return std::clock() / (CLOCKS_PER_SEC / 1000);
  }

  void write(pio_addr_t port, uint32_t data, SIZE size) {
   
  }
};

#endif

