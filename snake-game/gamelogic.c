/*
Game logic for snake game, gamelogic.c.
*/

#include <stdint.h>
#include "gameobjects.h"

Snake snake;					//Intialize struct
Apple apple;					//Intialize struct

extern void init_display();
extern void set_score();
extern int score;

int snake_tail[2];
int run;
int apple_exists = 0;
int score = 0;
int victory = 0;
unsigned int random_number_seed = 7269947;	//Arbitrary
volatile uint8_t* VGA = (volatile uint8_t*) VGA_SCREEN_BUF_BASE_ADDR;
volatile uint32_t* VGA_CTRL = (volatile uint32_t*) VGA_PIXEL_BUF_BASE_ADDR;


int get_sw()
{
	/*Returns value stored on address of first switch. --Alexander*/
	
	volatile uint32_t *sw = (volatile uint32_t *)SWITCH_BASE_ADDR;
	return (*sw) & GAME_SWITCH;
}

int get_btn()
{
	/*Returns value on btn 1, detects if pressed. --Alexander*/
	
	volatile uint8_t *btn = (volatile uint8_t *)BTN_BASE_ADDR;
	return (*btn) & BTN_MASK;
}

unsigned int pseudo_random_number_generator()
{
	/*Generates a PRN with the help of the seed defined above. --Magnus*/
	
	unsigned int offset = 81440;
	unsigned int base = 5097217;
	unsigned int bit_mask = 0x0fffff;
	
	random_number_seed = (base * random_number_seed + offset) & bit_mask;
	
	return random_number_seed;
}

void show_framebuffer()
{
	/*Updates the frambuffer and shows graphics onto the screen. --Alexander*/
	
	VGA_CTRL[1] = (uint32_t) VGA;	//Starts displaying from this address.
    VGA_CTRL[0] = 0;				//Triggers the swap.
}








void draw_block(int grid_x, int grid_y, uint8_t color)
{
	/*Draws a 20x20 pixel block of given color onto a 10x10 grid centered on the screen.
	The grid is composed of these blocks, 100 total and is the playing field. --Magnus*/
	
    int position_x = GRID_OFFSET_X + grid_x * BLOCK_SIZE;
    int position_y = GRID_OFFSET_Y + grid_y * BLOCK_SIZE;

    for (int y = 0; y < BLOCK_SIZE; y++) {
        for (int x = 0; x < BLOCK_SIZE; x++) {
            VGA[(position_y + y) * 320 + (position_x + x)] = color;	//Writes pixels to the linear screenbuffer.
        }
    }
}

void draw_letter_block(int grid_x, int grid_y, uint8_t color)
{	
	/*Draws a smaller 10x10 block with a different offset, used specifically for game over. --Magnus*/

	int position_x = TEXT_OFFSET_X + grid_x * LETTER_BLOCK_SIZE;
    int position_y = TEXT_OFFSET_y + grid_y * LETTER_BLOCK_SIZE;
	for (int y = 0; y < LETTER_BLOCK_SIZE; y++) {
        for (int x = 0; x < LETTER_BLOCK_SIZE; x++) {
            VGA[(position_y + y) * 320 + (position_x + x)] = color;
        }
    }
}

void draw_pixel_on_block(int grid_x, int grid_y, int pixel_offset_x, int pixel_offset_y, uint8_t color)
{
	/*Draws a single pixel on a block on the playing field (10x10 grid) that 
	has the coordinates (grid_x, grid_y). The pixel offset is where on the block the pixel is drawn. --Magnus*/
	
	int block_position_x = GRID_OFFSET_X + grid_x * BLOCK_SIZE;
    int block_position_y = GRID_OFFSET_Y + grid_y * BLOCK_SIZE;
	
	int pixel_block_position_x = block_position_x + pixel_offset_x;
	int pixel_block_position_y = block_position_y + pixel_offset_y;
	
	VGA[pixel_block_position_y * 320 + pixel_block_position_x] = color;
}

void draw_grid_block(int grid_x, int grid_y, uint8_t color)
{
	/*Draws the blocks that compose the 10x10 playing field of the snake.
	They are black with blue borders. --Magnus*/
	
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
	/*Specific function for drawing a G on a specific offset, part of GAMEOVER and WIN. --Magnus*/
	
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
	/*Specific function for drawing an A on a specific offset, part of GAMEOVER and WIN. --Magnus*/
	
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
	/*Specific function for drawing an M on a specific offset, part of GAMEOVER and WIN. --Magnus*/
	
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
	/*Specific function for drawing an E on a given offset, part of GAMEOVER and WIN. --Magnus*/
	
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
	/*Specific function for drawing an O on a specific offset, part of GAMEOVER and WIN. --Magnus*/
	
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
	/*Specific function for drawing a V on a specific offset, part of GAMEOVER and WIN. --Magnus*/
	
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
	/*Specific function for drawing an R on a specific offset, part of GAMEOVER and WIN. --Magnus*/
	
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
	/*Uses the letter drawing functions to compose the message GAME (above) and OVER (below).*/
	
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
	/*Fills the entire screen with a given color. --Magnus*/
	
	for (int i = 0; i < 320 * 240; i++) {
        VGA[i] = color;
    }
	show_framebuffer();
}

void WIN()
{
	/*Called if 99 apples are eaten, will end the game loop,
	create a black screen with the green letters GAME. The game can be reset simply
	with btn 1. --Magnus*/
	
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
	/*Creates the 10x10 grid for the snake to play on. --Magnus*/
	
	for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            draw_grid_block(x, y, color);
        }
    }
	show_framebuffer();
}

void draw_snake_head(uint8_t color)
{
	/*Draws the snake head. --Magnus*/
	
	draw_block(snake.body[0].x, snake.body[0].y, color);	
}

void draw_pineapple(int x, int y)
{
	/*Draws a pineapple on given grid position (x,y). --Magnus*/
	
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
	/*Assigns snake struct coordinates for the snake head, its direction, length and it is then drawn
	on said coordinates. --Magnus*/
	
	snake.body[0].x = 3;
	snake.body[0].y = 3;
	snake.length = 1;
	snake.direction = 'R';
	
	draw_block(snake.body[0].x, snake.body[0].y, color);	
	
	show_framebuffer();
}

void lengthen_snake(char direction)
{
	/*  increase length of snake body by a 1, and reassigns the tail to 
	one block behind the snake and draws a new snake segment there. Function
	also checks if snake is of length 100 (max). --Magnus*/
	
	snake.body[snake.length].x = snake_tail[0];
	snake.body[snake.length].y = snake_tail[1];
	snake.length += 1;
	draw_block(snake_tail[0], snake_tail[1], 0x1C);
	if(snake.length == MAX_SNAKE_LENGTH) victory = 1;
}

void increment_score()
{
    /*Increments the score and calls set_score() from display.c that updates
	the score onto the seven segment displays on the board. --Magnus*/
	
	score += 1;
	set_score(score);
}

void game_over()
{
    /*Exits the game loop, sets score to 0 and displays GAMEOVER. --Magnus*/
		
	run = 0;
	score = 0;
	create_background(0x00);
	draw_GAMEOVER();
	show_framebuffer();
}

void move_snake()
{
    /*Updates the position of the snake first and draws new tail,
	thereafter checks direction and redraws rest of snake.*/	
	
	
	snake_tail[0] = snake.body[snake.length-1].x;			//Tails is saved so that it is not forgotten
	snake_tail[1] = snake.body[snake.length-1].y;			// in following loop.
	draw_grid_block(snake_tail[0], snake_tail[1], 0x00);
	
	for(int i = snake.length - 1; i>0; i--)					//Reassign position of all snake segments tail to head.
	{
		snake.body[i] = snake.body[i-1];
	}
	switch(snake.direction)									//Updates head position based on direction.
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
	for(int i = 0; i < snake.length; i++)					//Redraw snake.
	{
		draw_block(snake.body[i].x, snake.body[i].y, 0x1C);
	}
}

void change_direction(char turn)
{
    /*Based on value of switch 1, snake turns right or left, this function changes direction
		depending on turn value when btn is pressed. --Magnus*/
	
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
	/*Checks if snake head coordinates are same as any other snake segment. --Magnus*/
	
	for(int i = 1; i < snake.length; i++)
	{
		if(snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) return 1;
	}
	return 0;
}

int is_head_outside_map()
{
	/*Checks if snake head coordinates fall outside of 10x10 playing field. --Magnus*/
	
	if((snake.body[0].x >= GRID_LIMIT || snake.body[0].x < 0) ||
		(snake.body[0].y >= GRID_LIMIT || snake.body[0].y < 0)) return 1;
	return 0;
}

int is_head_on_apple(Apple* apple)
{
	/*Checks if snake head coordinates are the same as apple. --Magnus*/
	
	if(snake.body[0].x == apple->x && snake.body[0].y == apple->y) return 1;
	return 0;
}

int is_block_snake(int x, int y)
{
	/*Checks if a block given by (x,y) is part of the snake or not. --Magnus*/
	
	for(int i = 0; i < snake.length; i++)
	{
		if(snake.body[i].x == x && snake.body[i].y == y) return 1;
	}
	return 0;
}

void spawn_apple(Apple* apple)
{	
	/*Uses the PRNG and is_block_snake to randomly spawn an apple on a grid position
		that is not occupied by snake. --Magnus*/

	int random_mod_ten = pseudo_random_number_generator()%10;	//random number mod 10 so that it can translate to grid coordinates.
	if(!apple_exists)
	{
		for(int i = 0; i < 10; i++)
		{
			int y = (random_mod_ten+i)%10;						//Calculate y coordinate
			for(int j = 0; j < 10; j++)
			{
				int x = (random_mod_ten+j)%10;					//Calculate x coordinate
				if(!is_block_snake(x, y))						//Check if randomly chosen block is snake
				{
					apple->x = x;								//Assign apple coordinates to block and draw pineapple
					apple->y = y;
					draw_pineapple(apple->x, apple->y);
					apple_exists = 1;							//Prevents apples from spawning
					return;
				}
			}
		}
	}
}

void create_first_apple(Apple* apple)
{
	/*Spawns the first apple on specific position and prevents apple spawn until it is eaten. --Magnus  */
	
	apple->x = 5;
	apple->y = 5;
	draw_pineapple(apple->x, apple->y);
	apple_exists = 1;
}

void check_collision(Apple* apple)
{
    /*Checks if snake head collides with: snake, outside of grid or apple and responds appropriately. --Magnus*/
		
	if(is_head_outside_map())	//End game if head is outside map or on body
	{
		game_over();
		return;
	}
	
	if(is_head_on_body())		//
	{
		game_over();
		return;
	}
	
	if(is_head_on_apple(apple))				//If head is on apple: 
	{	
		apple_exists = 0;					//enables apple spawns
		lengthen_snake(snake.direction);	
		increment_score();					
		while(!apple_exists)				//Continuously attempts to spawn apple until it succeeds.
		{										
			spawn_apple(apple);
		}		
	}

}

void game_init()
{
    /*Initialises the game, starts game loop run = 1, creates background and playing grid, sets the score on leds
		to 0, spawns apple and snake. --Magnus*/
	
	run = 1;
	create_background(0xE0);
	create_playing_field(0x00);
	init_display();
	create_first_apple(&apple);
	create_snake(0x1C);
}

void game()
{
    /*Checks for victory and collisions while moving the snake. --Magnus */
		
	move_snake();
	check_collision(&apple);
	show_framebuffer();
	if(victory) WIN();
}