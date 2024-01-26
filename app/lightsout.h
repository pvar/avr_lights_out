#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void mcu_init (void);
void app_init (void);
void testLedMatrix(void);

#define COLS 8
#define ROWS 5

#define SPK_PIN 5
#define BTN_PIN 5
