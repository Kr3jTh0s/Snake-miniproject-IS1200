/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include "gameobjects.h"

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";

int run = 1;
int timeoutcount = 0;

volatile char *_VGA = (volatile char *)VGA_SCREEN_BUF_BASE_ADDR;
volatile uint32_t *_VGA_CTRL = (volatile uint32_t *)VGA_PIXEL_BUF_BASE_ADDR;
unsigned int y_ofs = 0;

static inline int check_timeout()
{
  volatile uint32_t *timer_status = (volatile uint32_t *)(TIMER_BASE_ADDR);
  if ((*timer_status & 0x1) == 0x1)
  {
    *timer_status = *timer_status & 0xfffe;
    return 1;
  }
  return 0;
}

void handle_interrupt(unsigned cause)
{
  volatile uint32_t *timer_status = (volatile uint32_t *)TIMER_BASE_ADDR;
  volatile uint32_t *sw_edge = (volatile uint32_t *)(SWITCH_BASE_ADDR + SWITCH_EDGECAPTURE_OFFSET);

  if ((*sw_edge & 0x1) == 0x1)
  {
    delay(10);
    *sw_edge = *sw_edge & 0x3fe;
  }
  else if ((*timer_status & 0x1) == 0x1)
  {
    *timer_status = *timer_status & 0xfffe;
  }

  // --- update framebuffer ---
  for (int y = 0; y < 480; y++)
  {
    for (int x = 0; x < 320; x++)
    {
      _VGA[y * 320 + x] = (y + y_ofs) & 0xFF; // moving vertical gradient
    }
  }

  // tell VGA controller which buffer to display
  _VGA_CTRL[1] = (unsigned int)_VGA;
  _VGA_CTRL[0] = 1; // start swap
  while (_VGA_CTRL[3] & 0x1)
  {
  }; // wait until swap done
  y_ofs = (y_ofs + 1) % 240;
}

/* Add your code here for initializing interrupts. */
void labinit(void)
{
  volatile uint32_t *timer_periodl = (volatile uint32_t *)(TIMER_BASE_ADDR + TIMER_PERIODL_OFFSET);
  volatile uint32_t *timer_periodh = (volatile uint32_t *)(TIMER_BASE_ADDR + TIMER_PERIODH_OFFSET);
  volatile uint32_t *timer_control = (volatile uint32_t *)(TIMER_BASE_ADDR + TIMER_CONTROL_OFFSET);

  uint32_t period = 3000000;

  *timer_periodl = period & 0xffff;
  *timer_periodh = (period >> 16) & 0xffff;

  *timer_control = 0x7;

  enable_interrupt();
}

/* Your code goes into main as well as any needed functions. */
int main()
{
  labinit(); // initialize timer + interrupts

  while (1)
  {
    asm volatile("wfi");
  }
}