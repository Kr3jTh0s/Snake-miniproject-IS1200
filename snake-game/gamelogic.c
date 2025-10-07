/*
Game logic for snake game, gamelogic.c.
*/

#include <stdint.h>
#include "gameobjects.h"

Snake snake;
Apple apple;

extern void init_display();
extern void set_score();
extern int score;

int snake_tail[2];
int run;
int apple_exists = 0;
int score = 0;
int victory = 0;
unsigned int random_number_seed = 7269947;
volatile uint8_t* VGA = (volatile uint8_t*) VGA_SCREEN_BUF_BASE_ADDR;
volatile uint32_t* VGA_CTRL = (volatile uint32_t*) VGA_PIXEL_BUF_BASE_ADDR;


int get_sw()
{
  volatile uint32_t *sw = (volatile uint32_t *)SWITCH_BASE_ADDR;
  return (*sw) & GAME_SWITCH;
}

int get_btn()
{
  volatile uint8_t *btn = (volatile uint8_t *)BTN_BASE_ADDR;
  return (*btn) & BTN_MASK;
}

unsigned int pseudo_random_number_generator()
{
	unsigned int offset = 81440;
	unsigned int base = 5097217;
	unsigned int bit_mask = 0x0fffff;
	
	random_number_seed = (base * random_number_seed + offset) & bit_mask;
	
	return random_number_seed;
}

void show_framebuffer()
{
	VGA_CTRL[1] = (uint32_t) VGA;
    VGA_CTRL[0] = 0;
}








void draw_block(int grid_x, int grid_y, uint8_t color)
{
	
    int position_x = GRID_OFFSET_X + grid_x * BLOCK_SIZE;
    int position_y = GRID_OFFSET_Y + grid_y * BLOCK_SIZE;

    for (int y = 0; y < BLOCK_SIZE; y++) {
        for (int x = 0; x < BLOCK_SIZE; x++) {
            VGA[(position_y + y) * 320 + (position_x + x)] = color;
        }
    }
}

void draw_letter_block(int grid_x, int grid_y, uint8_t color)
{	
	int position_x = TEXT_OFFSET_X + grid_x * LETTER_BLOCK_SIZE;
    int position_y = TEXT_OFFSET_y + grid_y * LETTER_BLOCK_SIZE;
	for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            VGA[(position_y + y) * 320 + (position_x + x)] = color;
        }
    }
}

void draw_pixel_on_block(int grid_x, int grid_y, int pixel_offset_x, int pixel_offset_y, uint8_t color)
{
	int block_position_x = GRID_OFFSET_X + grid_x * BLOCK_SIZE;
    int block_position_y = GRID_OFFSET_Y + grid_y * BLOCK_SIZE;
	
	int pixel_block_position_x = block_position_x + pixel_offset_x;
	int pixel_block_position_y = block_position_y + pixel_offset_y;
	
	VGA[pixel_block_position_y * 320 + pixel_block_position_x] = color;
}

void draw_grid_block(int grid_x, int grid_y, uint8_t color)
{
	draw_block(grid_x, grid_y, color);
	for(int i = 0; i < 20; i++)
	{
		draw_pixel_on_block(grid_x, grid_y, i, 0,0x03);
		draw_pixel_on_block(grid_x, grid_y, 0, i,0x03);
		draw_pixel_on_block(grid_x, grid_y, i, 19,0x03);
		draw_pixel_on_block(grid_x, grid_y, 19, i,0x03);
	}
}

void draw_G(uint8_t color)
{
	for(int i = 3; i < 7; i++)
	{
		draw_letter_block(i, 3, color);
		draw_letter_block(i, 7, color);
	}
	for(int i = 4; i < 7; i ++)
	{
		draw_letter_block(2, i, color);
		draw_letter_block(i, 5, color);
	}
	draw_letter_block(6, 6, color);
}

void draw_A(uint8_t color)
{
	for(int i = 9; i < 12; i++)
	{
		draw_letter_block(i, 3, color);
	}
	for(int i = 8; i < 12; i++)
	{
		draw_letter_block(i, 5, color);
		draw_letter_block(8, i-4, color);
		draw_letter_block(12, i-4, color);
	}
	
}

void draw_M(uint8_t color)
{
	for(int i = 3; i < 8; i++)
	{
		draw_letter_block(14, i, color);
		draw_letter_block(18, i, color);
	}
	draw_letter_block(15, 4, color);
	draw_letter_block(16, 5, color);
	draw_letter_block(17, 4, color);
}

void draw_E(int x_offset, int y_offset, uint8_t color)
{
	for(int i = 3 + y_offset; i < 8 + y_offset; i++)
	{
		draw_letter_block(20 + x_offset, i, color);
	}
	for(int i = 21 + x_offset; i < 25 + x_offset; i++)
	{
		draw_letter_block(i, 3 + y_offset, color);
		draw_letter_block(i, 5 + y_offset, color);
		draw_letter_block(i, 7 + y_offset, color);
	}
}

void draw_O()
{
	for(int i = 3; i < 6; i++)
	{
		draw_letter_block(i, 9, 0xE0);
		draw_letter_block(i, 13, 0xE0);
	}
	
	for(int i = 10; i < 13; i++)
	{
		draw_letter_block(2, i, 0xE0);
		draw_letter_block(6, i, 0xE0);
	}
}

void draw_V()
{
	for(int i = 9; i < 12; i++)
	{
		draw_letter_block(8, i, 0xE0);
		draw_letter_block(12, i, 0xE0);
	}
	draw_letter_block(9, 12, 0xE0);
	draw_letter_block(11, 12, 0xE0);
	draw_letter_block(10, 13, 0xE0);
}

void draw_R()
{
	for(int i = 9; i < 14; i++)
	{
		draw_letter_block(20, i, 0xE0);
	}
	for(int i = 21; i < 25; i++)
	{
		draw_letter_block(i, 9, 0xE0);
		draw_letter_block(i, 11, 0xE0);
	}
	draw_letter_block(24, 10, 0xE0);
	draw_letter_block(23, 12, 0xE0);
	draw_letter_block(24, 13, 0xE0);
}

void draw_GAMEOVER()
{
	draw_G(0xE0);
	draw_A(0xE0);
	draw_M(0xE0);
	draw_E(0, 0, 0xE0);
	
	draw_O();
	draw_V();
	draw_E(-6, 6, 0xE0);
	draw_R();
}

void create_background(uint8_t color)
{
	for (int i = 0; i < 320 * 240; i++) {
        VGA[i] = color;
    }
	show_framebuffer();
}

void WIN()
{
	run = 0;
	create_background(0x00);
	draw_G(0x18);
	draw_A(0x18);
	draw_M(0x18);
	draw_E(0, 0, 0x18);
	show_framebuffer();
}

void create_playing_field(uint8_t color)
{
	for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            draw_grid_block(x, y, color);
        }
    }
	show_framebuffer();
}

void draw_snake_head(uint8_t color)
{
	draw_block(snake.body[0].x, snake.body[0].y, color);
	
}

void draw_pineapple(int x, int y)
{
	for(int i = 10; i < 19; i++)
	{
		draw_pixel_on_block(x, y, 7, i, 0xFC);
		draw_pixel_on_block(x, y, 8, i, 0xFC);
		draw_pixel_on_block(x, y, 9, i, 0xFC);
		draw_pixel_on_block(x, y, 10, i, 0xFC);
		draw_pixel_on_block(x, y, 11, i, 0xFC);
		draw_pixel_on_block(x, y, 12, i, 0xFC);
	}
	
	for(int i = 7; i < 10; i++)
	{
		draw_pixel_on_block(x, y, 9, i, 0x1C);
		draw_pixel_on_block(x, y, 10, i, 0x1C);
	}
	for(int i = 7; i < 9; i++)
	{
		draw_pixel_on_block(x, y, i, i+1, 0x1C);
	}
	draw_pixel_on_block(x, y, 11, 9, 0x1C);
	draw_pixel_on_block(x, y, 12, 8, 0x1C);

	
}

void create_snake(uint8_t color)
{
	
	snake.body[0].x = 3;
	snake.body[0].y = 3;
	snake.length = 1;
	snake.direction = 'R';
	
	draw_block(snake.body[0].x, snake.body[0].y, color);	
	
	show_framebuffer();
}

void lengthen_snake(char direction)
{
	/*  increase length of snake body by a constant amount
        snake.length += 1 */
	
	//
	snake.body[snake.length].x = snake_tail[0];
	snake.body[snake.length].y = snake_tail[1];
	snake.length += 1;
	draw_block(snake_tail[0], snake_tail[1], 0x1C);
	if(snake.length == MAX_SNAKE_LENGTH) victory = 1;
}

void increment_score()
{
    /*  */
	score += 1;
	set_score(score);
}

void game_over()
{
    /*  */
		
	run = 0;
	score = 0;
	create_background(0x00);
	draw_GAMEOVER();
	show_framebuffer();
}

void move_snake()
{
    /*  */	
	
	
	snake_tail[0] = snake.body[snake.length-1].x;
	snake_tail[1] = snake.body[snake.length-1].y;
	draw_grid_block(snake_tail[0], snake_tail[1], 0x00);
	
	for(int i = snake.length - 1; i>0; i--)
	{
		snake.body[i] = snake.body[i-1];
	}
	switch(snake.direction)
	{
		case 'R':
			snake.body[0].x += 1;
			break;
		case 'L':
			snake.body[0].x -= 1;
			break;
		case 'U':
			snake.body[0].y -= 1;
			break;
		case 'D':
			snake.body[0].y += 1;
			break;
	}
	for(int i = 0; i < snake.length; i++)
	{
		draw_block(snake.body[i].x, snake.body[i].y, 0x1C);
	}
}

void change_direction(char turn)
{
    /* */
	
	if(turn == 'R')
	{
		switch(snake.direction)
		{
			case 'R':
				snake.direction = 'D';
				break;
			case 'L':
				snake.direction = 'U';
				break;
			case 'U':
				snake.direction = 'R';
				break;
			case 'D':
				snake.direction = 'L';
				break;
		}
	}
	else if(turn == 'L')
	{
		switch(snake.direction)
		{
			case 'R':
				snake.direction = 'U';
				break;
			case 'L':
				snake.direction = 'D';
				break;
			case 'U':
				snake.direction = 'L';
				break;
			case 'D':
				snake.direction = 'R';
				break;
		}
	}
}

int is_head_on_body()
{
	for(int i = 1; i < snake.length; i++)
	{
		if(snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) return 1;
	}
	return 0;
}

int is_head_outside_map()
{
	if((snake.body[0].x >= GRID_LIMIT || snake.body[0].x < 0) ||
		(snake.body[0].y >= GRID_LIMIT || snake.body[0].y < 0)) return 1;
	return 0;
}

int is_head_on_apple(Apple* apple)
{
	if(snake.body[0].x == apple->x && snake.body[0].y == apple->y) return 1;
	return 0;
}

int is_block_snake(int x, int y)
{
	for(int i = 0; i < snake.length; i++)
	{
		if(snake.body[i].x == x && snake.body[i].y == y) return 1;
	}
	return 0;
}

void spawn_apple(Apple* apple)
{	

	int random_mod_ten = pseudo_random_number_generator()%10;
	if(!apple_exists)
	{
		for(int i = 0; i < 10; i++)
		{
			int y = (random_mod_ten+i)%10;
			for(int j = 0; j < 10; j++)
			{
				int x = (random_mod_ten+j)%10;
				if(!is_block_snake(x, y))
				{
					apple->x = x;
					apple->y = y;
					draw_pineapple(apple->x, apple->y);
					apple_exists = 1;
					return;
				}
			}
		}
	}
}

void create_first_apple(Apple* apple)
{
	/*  */
	
	apple->x = 5;
	apple->y = 5;
	draw_pineapple(apple->x, apple->y);
	apple_exists = 1;
}

void check_collision(Apple* apple)
{
    /*  */
		
	if(is_head_outside_map())
	{
		game_over();
		return;
	}
	
	if(is_head_on_body())
	{
		game_over();
		return;
	}
	
	if(is_head_on_apple(apple))
	{	
		apple_exists = 0;
		lengthen_snake(snake.direction);
		increment_score();
		while(!apple_exists)
		{
			spawn_apple(apple);
		}		
	}

}

void game_init()
{
    /*  */
	
	run = 1;
	create_background(0xE0);
	create_playing_field(0x00);
	init_display();
	create_first_apple(&apple);
	create_snake(0x1C);
}

void game()
{
    /*  */
		
	move_snake();
	check_collision(&apple);
	show_framebuffer();
	if(victory) WIN();
}