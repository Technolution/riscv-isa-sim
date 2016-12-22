/* This file implements a minimal 8250 UART block. */
#include "devices.h"
#include "processor.h"
#include <unistd.h>

plic_t::plic_t(std::vector<processor_t*>& procs)
  : procs(procs)
{
  this->ie = 0;
  this->pending = 0;
  this->thres = 0;
}
bool plic_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  uint32_t* word = (uint32_t*)bytes;
  uint32_t status = 0;
  if (len != 4)
    return false;

  switch (addr)
  {
    case 0x0:  // interrupt enable
      *word = this->ie;    // empty
      return true;
    case 0x80:  // priority
      *word = this->priority[0];
      return true;
    case 0x84:
      *word = this->priority[1];
      return true;
    case 0x100:  // set the threshold
      *word = this->thres;
      return true;
    case 0x104:  // basic claim for interrupt 1
      status = this->pending & this->ie & (~this->claimed);
      if (status & 0x02) {
          *word = 1;
          this->claimed |= 0x02;
      } else {
          *word = 0;
      }
      return true;
    default:
      return false;
  }
  return true;
}

bool plic_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  uint32_t* word = (uint32_t*)bytes;
  if (len != 4)
    return false;
  switch (addr)
  {
      case 0x0:  // interrupt enable
        this->ie = *word & 0x03; // only the bottom two bits
        return true;
      case 0x80:  // priority
        this->priority[0] = *word & 0x01;
        return true;
      case 0x84:
        this->priority[1] = *word & 0x01;
        return true;
      case 0x100:  // set the threshold
        this->thres = *word & 0x01;
        return true;
      case 0x104:  // clear the claimed flag
        this->claimed &= ~(1UL << *word);
        return true;
      default:
        return false;
  }
  return true;
}

void plic_t::set_interrupt( uint32_t irq )
{
  this->pending |= 1UL << irq;
}

void plic_t::clear_interrupt( uint32_t irq )
{
  this->pending &= ~(1UL << irq);
}
