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


#include "gameobjects.h"
#include <stdint.h>






void game_init(){
    /*  initialize game values such as snake length and such
        snake.length = const
        snake.direction = R
        score = 0
        ... */
}

void game(){
    /*  main function. contains loop that runs the game
        move_snake()
        check_collision() */
}

/* Below is the function that will be called when an interrupt is triggered. */
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

void move_snake(){
    /*  makes the snake continuously move in a specified direction
        (U, D, L, R) (Up, Down, Left, Right) */
}

void change_direction(){
    /*  change the direction that the snake is continuously moving
        by a 90 degree turn of its current direction
        (U, D, L, R) (Up, Down, Left, Right) 
        snake.direction = 'L' */
}

void check_collision(){
    /*  check if any pixel of the snake's head is overlapping with 
        itself, the out of bounds border or with an apple.
        
        display game over if head collides with body or out of 
        bounds.
        game_over()
        
        add and display score if head collides with an apple. also
        increase snake body length. 
        add_score()
        increase_length() */
}

void add_score(){
    /*  adds score of eaten apple to a value that is displayed on 
        7 segment display */ 
}

void increase_length(){
    /*  increase length of snake body by a constant amount
        snake.length += const */
}

void game_over(){
    /*  pause snake movement, make the score LEDs flicker and
        display "game over" screen */
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
}