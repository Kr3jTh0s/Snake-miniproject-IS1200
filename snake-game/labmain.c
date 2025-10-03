/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include "gameobjects.h"
#include <stdio.h>

void game();
void game_init();
void draw_block();
void change_direction();

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";
extern int run;

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
  volatile uint32_t* sw_data = (volatile uint32_t*)SWITCH_BASE_ADDR;
  volatile uint32_t* btn_edge = (volatile uint32_t*)(BTN_BASE_ADDR + 0xC);

  if ((*sw_edge & 0x1) == 0x1)
  {
    delay(10);
    *sw_edge = *sw_edge & 0x3fe;
  }
  
  else if ((*timer_status & 0x1) == 0x1)
  {
    *timer_status = *timer_status & 0xfffe;
  }
  
  if ((*btn_edge & 0x1) == 0x1)
    {
		delay(200);
		volatile uint32_t* led = (volatile uint32_t*)LED_BASE_ADDR;
		*led ^= 0x1;  // Toggle LED 0
        uint32_t sw_val = *sw_data;

        if ((sw_val & 0x1) == 0x1)
        {
            change_direction('L');  // Left turn
        }
        else
        {
            change_direction('R');  // Right turn
        }

        *btn_edge = 0x1;  // Clear the interrupt
    }



/*
  // --- update framebuffer ---
  for (int y = 0; y < 240; y++)
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

  _VGA_CTRL[1] = (unsigned int)_VGA;
  _VGA_CTRL[0] = 1; // start swap*/
}

/* Add your code here for initializing interrupts. */
void labinit(void)
{
	volatile uint32_t *timer_periodl = (volatile uint32_t *)(TIMER_BASE_ADDR + TIMER_PERIODL_OFFSET);
	volatile uint32_t *timer_periodh = (volatile uint32_t *)(TIMER_BASE_ADDR + TIMER_PERIODH_OFFSET);
	volatile uint32_t *timer_control = (volatile uint32_t *)(TIMER_BASE_ADDR + TIMER_CONTROL_OFFSET);
	uint32_t period = 30000000;

	*timer_periodl = period & 0xffff;
	*timer_periodh = (period >> 16) & 0xffff;

	*timer_control = 0x7;

	volatile uint32_t* btn_interruptmask = (volatile uint32_t*)(BTN_BASE_ADDR + 0x8);
	volatile uint32_t* btn_edgecapture = (volatile uint32_t*)(BTN_BASE_ADDR + 0xC);

	*btn_edgecapture = 1;
	*btn_interruptmask = 1;

	enable_interrupt();
}

/* Your code goes into main as well as any needed functions. */
int main()
{

  	labinit(); // initialize timer + interrupts
	game_init();
	delay(3000);
    while (run) {
        game();
		delay(2500);
    }
    //game_over();
    /*while(1) {
        asm volatile("wfi");
    }*/
}