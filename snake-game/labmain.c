/*
Main file for snake game, labmain.c.
*/

#include <stdint.h>
#include "gameobjects.h"
#include <stdio.h>

extern int run;
extern volatile uint8_t* VGA;
extern volatile uint32_t* VGA_CTRL;

volatile int move_snake_mark = 0;		

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
  volatile uint32_t* sw_data = (volatile uint32_t*)SWITCH_BASE_ADDR;
  volatile uint32_t* btn_edge = (volatile uint32_t*)(BTN_BASE_ADDR + 0xC);

  
	if ((*timer_status & 0x1) == 0x1)
	{
		move_snake_mark = 1;
		*timer_status = *timer_status & 0xfffe;
	}

	if ((*btn_edge & 0x1) == 0x1)
	{
		uint32_t sw_val = *sw_data;

		if ((sw_val & 0x1) == 0x1)
		{
			change_direction('L');  //Left turn
		}
		
		else
		{
			change_direction('R');  //Right turn
		}
	*btn_edge = 0x1;  //Clear the interrupt
	
	}
}

void init(void)
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


int main()
{

  	init();
	while(1)
	{
		game_init();
		while (run) 
		{
			if(move_snake_mark)
			{
				move_snake_mark = 0;
				game();
			}
		}
		
		while(get_btn() == 0)
		{
			
		}
		run = 1;
	}

}