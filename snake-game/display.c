#include <stdint.h>
#include "gameobjects.h"

/*
int countLED = 0x0;

// Seven-segment display encoding for digits 0-9
const uint8_t segm7_display[10] = {
    0b01000000, // 0
    0b01111001, // 1
    0b00100100, // 2
    0b00110000, // 3
    0b00011001, // 4
    0b00010010, // 5
    0b00000010, // 6
    0b01111000, // 7
    0b00000000, // 8
    0b00011000  // 9
};

inline void set_LEDs(int led_mask)
{
  volatile int *led = (volatile int *)LED_BASE_ADDR;
  *led = led_mask;
}

inline void set_displays(int display_number, int value)
{
  volatile uint8_t *display = (volatile uint8_t *)(DISPLAY_BASE_ADDR + (display_number)*DISPLAY_OFFSET);
  *display = segm7_display[value];
}

inline void update_displays(int my_time)
{
  for (int display_number = 0; display_number < MAX_DISPLAY_NUM; display_number++)
  {
    int rest = my_time % 16;
    set_displays(display_number, rest);
    my_time = my_time / 16;
  }
}

inline void count_LEDs(int my_time)
{
  update_displays(my_time);

  set_LEDs(countLED);
  if (countLED == 0xf)
  {
    display_modifier();
  }
  countLED++;
}


int VGA_function() // placeholder namn
{
  volatile char *VGA = (volatile char *)VGA_SCREEN_BUF_BASE_ADDR;
  for (int i = 0; i < 320 * 480; i++)
    VGA[i] = i / 320;
  unsigned int y_ofs = 0;
  volatile int *VGA_CTRL = (volatile int *)VGA_PIXEL_BUF_BASE_ADDR;
  while (1)
  {
    *(VGA_CTRL + 1) = (unsigned int)(VGA + y_ofs * 320);
    *(VGA_CTRL + 0) = 0;
    y_ofs = (y_ofs + 1) % 240;
    for (int i = 0; i < 1000000; i++)
      delay(10); // asm volatile("nop"); <- original code
  }
}*/