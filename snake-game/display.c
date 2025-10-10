/*Magnus + Alexander*/
#include <stdint.h>
#include "gameobjects.h"

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

void set_displays(int display_number, int value)
{
  volatile uint8_t *display = (volatile uint8_t *)(DISPLAY_BASE_ADDR + (display_number)*DISPLAY_OFFSET);
  *display = segm7_display[value];
}

void init_display()
{
	for(int i = 0; i < 6; i++)
	{
		set_displays(i, 0);
	}
}

void set_score(int score)
{
	int temp_score = score;
	for(int i = 0; i < 3; i++)
	{
		int number_for_display = temp_score % 10;
		set_displays(i, number_for_display);
		temp_score = temp_score / 10;
	}
}
