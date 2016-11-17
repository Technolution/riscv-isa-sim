/* This file implements a minimal 8250 UART block. */
#include "devices.h"
#include "processor.h"
#include <unistd.h>

bool uart_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  uint32_t* word = (uint32_t*)bytes;
  if (len != 4)
    return false;

  switch (addr)
  {
    case 0x14:  // tx status
      *word = 2;    // empty
      return true;
    case 0x18:  // tx count
      *word = 0;    // empty
      return true;
    case 0x1C:  // tx free
      *word = 1024; // a lot
      return true;
    case 0x20:  // rx data
      return false;
    case 0x24:  // rx status
      *word = 2;    // empty
      return true;
    case 0x28:  // rx count
      *word = 0;    // empty
      return true;
    case 0x2C:  // rx free
      *word = 1024; // a lot
      return true;
    default:
      return false;
  }
}

bool uart_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  uint32_t* word = (uint32_t*)bytes;
  if (len != 4)
    return false;

  switch (addr)
  {
    case 0x10:  // tx data
      putchar((*word) & 0xFF);
      fflush(stdout);
      break;
  }

  return true;
}
