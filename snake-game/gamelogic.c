//def spawn_apple
//def create_snake
//def check_collision
//def death_event
//def snake_position
//def move_snake
//def apple_consumed
//def victory
//def 
/*
get_sw
get_btn
get_time
get_vga
set_color
set_time
*/
/*def main{
  initialize:
    create game screen
    create snake
    spawn first apple
    start timer and interrupts
  while(1) {
    check for collision with:
      apple
      snake
      wall
    move snake: 
      interrupts and timer
      snake position
      button press
      switch value
    spawn new apple when current eaten:
      snake position
      apple position
      spawn apple
    check for victory:
      unable to spawn apple/snake certain length (entire screen)

  }
}*/

/*
Game logic for snake game, gamelogic.c.
*/

#include <stdint.h>
#include "gameobjects.h"

//kan flytta på.
int run = 1;
int apple_exists = 0;
volatile uint8_t* VGA = (volatile uint8_t*) VGA_SCREEN_BUF_BASE_ADDR;
volatile uint32_t* VGA_CTRL = (volatile uint32_t*) VGA_PIXEL_BUF_BASE_ADDR;

Snake snake;
Apple apple;
int snake_tail[2];

static inline int get_sw()
{
  volatile uint32_t *sw = (volatile uint32_t *)SWITCH_BASE_ADDR;
  return (*sw) & GAME_SWITCH;
}

static inline int get_btn()
{
  volatile uint8_t *btn = (volatile uint8_t *)BTN_BASE_ADDR;
  return (*btn) & BTN_MASK;
}



void show_framebuffer(){
	VGA_CTRL[1] = (uint32_t) VGA;
    VGA_CTRL[0] = 0;
}

void draw_block(int grid_x, int grid_y, uint8_t color){
	
    int position_x = GRID_OFFSET_X + grid_x * BLOCK_SIZE;
    int position_y = GRID_OFFSET_Y + grid_y * BLOCK_SIZE;

    for (int y = 0; y < BLOCK_SIZE; y++) {
        for (int x = 0; x < BLOCK_SIZE; x++) {
            VGA[(position_y + y) * 320 + (position_x + x)] = color;
        }
    }
}


void create_background(uint8_t color){
	for (int i = 0; i < 320 * 240; i++) {
        VGA[i] = color;
    }
	show_framebuffer();
}
void create_field(uint8_t color){
	for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            draw_block(x, y, color);
        }
    }
	show_framebuffer();
}

void create_snake(int color){
	
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
}

void move_snake(){
    /*  makes the snake continuously move in a specified direction
        (U, D, L, R) (Up, Down, Left, Right) */
	
	
	
	snake_tail[0] = snake.body[snake.length-1].x;
	snake_tail[1] = snake.body[snake.length-1].y;
	draw_block(snake_tail[0], snake_tail[1], 0x00);
	
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

void change_direction(char turn){
    /*  change the direction that the snake is continuously moving
        by a 90 degree turn of its current direction
        (U, D, L, R) (Up, Down, Left, Right) 
        snake.direction = 'L' */
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

int is_block_snake(int x, int y)
{
	for(int i = 0; i < snake.length; i++)
	{
		if(snake.body[i].x == x && snake.body[i].y == y) return 1;
	}
	return 0;
}

void spawn_apple(Apple* apple, int color)
{
	if(!apple_exists)
	{
		for(int y = 0; y < 10; y++)
		{
			for(int x = 0; x < 10; x++)
			{
				if(!is_block_snake(x, y))
				{
					apple->x = x;
					apple->y = y;
					draw_block(apple->x, apple->y, color);
					return;
				}
			}
		}
	}
}

void create_apple(Apple* apple, int color)
{
	apple->x = 5;
	apple->y = 5;
	draw_block(apple->x, apple->y, color);
	apple_exists = 1;
}

void check_collision(Apple* apple){
    /*  check if any pixel of the snake's head is overlapping with 
        itself, the out of bounds border or with an apple.
        
        display game over if head collides with body or out of 
        bounds.
        game_over()
        
        add and display score if head collides with an apple. also
        increase snake body length. 
        add_score()
        increase_length() */
	if((snake.body[0].x >= GRID_LIMIT || snake.body[0].x < 0) ||
		(snake.body[0].y >= GRID_LIMIT || snake.body[0].y < 0)){
		run = 0;
	}
	if(snake.body[0].x == apple->x && snake.body[0].y == apple->y)
	{	
		apple_exists = 0;
		spawn_apple(apple, 0xEC);
		lengthen_snake(snake.direction);		
	}

}

void add_score(){
    /*  adds score of eaten apple and update 7 segment display to
        display score */ 
}

void game_over(){
    /*  pause snake movement, make the score LEDs flicker and
        display "game over" screen */
}

void game_init(){
    /*  initialize game values such as snake length and such
        snake.length = const
        snake.direction = R
        score = 0
        ... */
		create_background(0xE0);
		create_field(0x00);//svart bakgrund
		create_apple(&apple, 0xEC);
		create_snake(0x1C);
}

void game(){
    /*  main function. contains loop that runs the game
        move_snake()
        check_collision() */
		//att testa
		move_snake();
		check_collision(&apple);
		show_framebuffer();
}

/* Below is the function that will be called when an interrupt is triggered. */
/*
void handle_interrupt(unsigned cause)
{
  volatile uint32_t *timer_status = (volatile uint32_t *)TIMER_BASE_ADDR;
  volatile uint32_t *sw_edge = (volatile uint32_t *)(SWITCH_BASE_ADDR + SWITCH_EDGECAPTURE_OFFSET);
  
  if ((*sw_edge & 0x1) == 0x1)
  {
    delay(10);
    *sw_edge = *sw_edge & 0x3fe;
    tick(&mytime);
    update_displays(mytime);
  }
  else if ((*timer_status & 0x1) == 0x1)
  {
    *timer_status = *timer_status & 0xfffe;

    if (timeoutcount == 10)
    {
      tick(&mytime);
      update_displays(mytime);
      timeoutcount = 0;
    }
    timeoutcount++;
  }
}


static inline int get_sw()
{
  volatile uint32_t *sw = (volatile uint32_t *)SWITCH_BASE_ADDR;
  return (*sw) & SWITCH_MASK;
}

void print_sw()
{
  int sw = get_sw();
  print(" | ");
  print_dec(sw);
  print(" sw | ");
}

static inline int get_btn()
{
  volatile uint8_t *btn = (volatile uint8_t *)BTN_BASE_ADDR;
  return (*btn) & BTN_MASK;
}

void print_btn()
{
  int btn = get_btn();
  print_dec(btn);
  print(" btn | ");
}


static inline void display_modifier()
{
  int time = 0x000000;
  while (1)
  {
    if (get_btn())
    {
      change_time(get_sw(), &time);
    }

    if (check_timeout())
    {
      if (timeoutcount == 10)
      {
        update_displays(time);
        tick(&time);
        timeoutcount = 0;
      }
      timeoutcount++;
    }
  }
}

static inline int to_hex_time(int time)
{
  int right_char = time % 10;
  int left_char = (time - right_char) / 10;
  return (left_char << 4) | right_char;
}

static inline void change_time(int sw, int *time)
{
  int right_most_sw = sw & 0x3f;
  if ((sw >> 8) == 0x1 && right_most_sw < MAX_SWITCH_VAL) // modify second counter
  {
    *time = (*time & 0xffff00) | to_hex_time(right_most_sw);
  }
  else if ((sw >> 8) == 0x2 && right_most_sw < MAX_SWITCH_VAL) // modify minute counter
  {
    *time = (*time & 0xff00ff) | (to_hex_time(right_most_sw) << 8);
  }
  else if ((sw >> 8) == 0x3) // modify hour counter
  {
    *time = (*time & 0x00ffff) | (to_hex_time(right_most_sw) << 16);
  }
}*/