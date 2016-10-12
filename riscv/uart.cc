/* This file implements a minimal 8250 UART block. */
#include "devices.h"
#include "processor.h"
#include <unistd.h>

enum {
  UART_DATA = 0,                  /* Data RX/TX */
  UART_INTR_EN,                   /* Interrupt enable */
  UART_INTR_ID,                   /* Interrupt identification */
  UART_LINE_CTL,                  /* Line control */
  UART_MODEM_CTL,                 /* Modem control */
  UART_LINE_STAT,                 /* Line status */
  UART_MODEM_STAT,                /* Modem status */
  UART_SCRATCH                    /* Scratch register */
};

enum {
  UART_LINE_STAT_THRE = (1 << 5), /* Transmitter holding register empty */
  UART_LINE_STAT_TEMT = (1 << 6), /* Transmitter completely empty */
};

bool uart_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  /* For simplicity on our side, only support byte-sized accesses for now. */
  if (len > 1)
    return false;

  switch (addr)
  {
    case UART_LINE_STAT:
      *bytes = UART_LINE_STAT_TEMT | UART_LINE_STAT_THRE;
      break;
    default:
      *bytes = 0;
  }

  return true;
}

bool uart_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  if (len > 1)
    return false;

  switch (addr)
  {
    case UART_DATA:
      putchar(*bytes);
      fflush(stdout);
      break;
  }

  return true;
}