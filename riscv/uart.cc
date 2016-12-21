/* This file implements a minimal 8250 UART block. */
#include "devices.h"
#include "processor.h"
#include <unistd.h>

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

uart_t::uart_t(std::vector<processor_t*>& procs)
  : procs(procs)
{
   plic = NULL;
}

void uart_t::add_plic(plic_t* plic_ref) {
    plic = plic_ref;
}

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
      if(kbhit()){
        *word = getchar();
        procs[0]->state.mip &= ~MIP_SEIP;
        if (plic)
           plic->clear_interrupt(1);
        return true;
      }
      return false;
    case 0x24:  // rx status
      *word = 2;    // empty
      return true;
    case 0x28:  // rx count
      *word = kbhit() ? 1 : 0;
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

/* getchar_int
 * 
 * Generates an interrupt for getchar
 * */
void uart_t::getchar_int()
{
  if(kbhit())
    procs[0]->state.mip |= MIP_SEIP;
    if (plic)
        plic->set_interrupt(1);
}
