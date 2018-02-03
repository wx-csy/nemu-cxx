#include "common.h"
#include "CPU/CPU.hpp"
#include "Memory.hpp"

CPU::CPU(Memory& memory):mmu(memory) {}

