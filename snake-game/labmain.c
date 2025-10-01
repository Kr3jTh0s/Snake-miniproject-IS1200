/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include "gameobjects.h"
#include "gamelogic.c"

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";

int countLED = 0x0;
int run = 1;
int timeoutcount = 0;

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
  // Call labinit()
  labinit();

  VGA_function();

  // Enter a forever loop
  // while (1)
  // {
  //   game();
  // }
}