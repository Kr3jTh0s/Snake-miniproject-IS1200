/* Below functions are external and found in other files. */
extern void print(const char *);
extern void print_dec(unsigned int);
extern void display_string(char *);
extern void time2string(char *, int);
extern void tick(int *);
extern void delay(int);
extern int nextprime(int);
extern void enable_interrupt(void);

// Hardware base addresses and offset
#define LED_BASE_ADDR 0x04000000
#define DISPLAY_BASE_ADDR 0x04000050
#define DISPLAY_OFFSET 0x10
#define MAX_DISPLAY_NUM 6
#define SWITCH_BASE_ADDR 0x04000010
#define SWITCH_MASK 0x3ff
#define SWITCH_INTERRUPTMASK_OFFSET 0x8
#define SWITCH_EDGECAPTURE_OFFSET 0xc
#define MAX_SWITCH_VAL 0x3c // 0x3c is the maximum value for the rightmost 6 switches (0-59 in decimal)
#define BTN_BASE_ADDR 0x040000d0
#define BTN_MASK 0x1
#define TIMER_BASE_ADDR 0x04000020
#define TIMER_PERIODL_OFFSET 0x8
#define TIMER_PERIODH_OFFSET 0xc
#define TIMER_CONTROL_OFFSET 0x4
#define VGA_PIXEL_BUF_BASE_ADDR 0x04000100
#define VGA_SCREEN_BUF_BASE_ADDR 0x08000000
#define VGA_WIDTH 320
#define VGA_HEIGHT 240
#define BLOCK_WIDTH 32
#define BLOCK_HEIGHT 24
#define VGA_BASE 0x08000000
#define VGA_CTRL_BASE 0x04000100
#define MAX_SNAKE_LENGTH 100

int countLED;
int timeoutcount;
int mytime;

void handle_interrupt(unsigned cause);
int VGA_function();
void display_modifier();

typedef struct Point{
    int x;
    int y;
} Point;

typedef struct Snake {
   int length;
   char direction;
   Point body[MAX_SNAKE_LENGTH];
} Snake;