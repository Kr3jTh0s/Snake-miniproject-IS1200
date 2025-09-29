/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

/* Below functions are external and found in other files. */
extern void print(const char *);
extern void print_dec(unsigned int);
extern void display_string(char *);
extern void time2string(char *, int);
extern void tick(int *);
extern void delay(int);
extern int nextprime(int);
extern void enable_interrupt(void);

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";

#include <stdint.h>

// Hardware base addresses and offset
#define LED_BASE_ADDR 0x04000000
#define DISPLAY_BASE_ADDR 0x04000050
#define DISPLAY_OFFSET 0x10
#define MAX_DISPLAY_NUM 6
#define SWITCH_BASE_ADDR 0x04000010
#define SWITCH_MASK 0x3ff
#define MAX_SWITCH_VAL 0x3c // 0x3c is the maximum value for the rightmost 6 switches (0-59 in decimal)
#define BTN_BASE_ADDR 0x040000d0
#define BTN_MASK 0x1
#define TIMER_BASE_ADDR 0x04000020
#define TIMER_PERIODL_OFFSET 0x8
#define TIMER_PERIODH_OFFSET 0xc
#define TIMER_CONTROL_OFFSET 0x4

// Seven-segment display encoding for digits 0-9
static const uint8_t segm7_display[10] = {
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

int countLED = 0x0;
int run = 1;
int timeoutcount = 0;
int prime = 1234567;

static inline void set_LEDs(int led_mask)
{
  volatile int *led = (volatile int *)LED_BASE_ADDR;
  *led = led_mask;
}

static inline void set_displays(int display_number, int value)
{
  volatile uint8_t *display = (volatile uint8_t *)(DISPLAY_BASE_ADDR + (display_number)*DISPLAY_OFFSET);
  *display = segm7_display[value];
}

static inline void update_displays(int my_time)
{
  for (int display_number = 0; display_number < MAX_DISPLAY_NUM; display_number++)
  {
    int rest = my_time % 16;
    set_displays(display_number, rest);
    my_time = my_time / 16;
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

static inline void count_LEDs(int my_time)
{
  update_displays(my_time);

  set_LEDs(countLED);
  if (countLED == 0xf)
  {
    display_modifier();
  }
  countLED++;
}

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause)
{
  volatile uint32_t *timer_status = (volatile uint32_t *)TIMER_BASE_ADDR;
  *timer_status = *timer_status & 0xfffe;

  if (timeoutcount == 10)
  {
    update_displays(mytime);
    tick(&mytime);
    timeoutcount = 0;
  }
  timeoutcount++;
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

  // Enter a forever loop
  while (1)
  {
    print("Prime : ");
    prime = nextprime(prime);
    print_dec(prime);
    print("\n");
  }
}
