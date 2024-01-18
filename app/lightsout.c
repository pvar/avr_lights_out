/*
 *
 */

#include <util/delay.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define TIME_TO_CHANGE 65500

uint8_t init;
uint8_t ledRow[5];

int main(void)
{
        // turn all LEDs off
        uint8_t i;
        for (i = 0; i < 5; i++) {
                ledRow[i] = 0;
        }

        init = 1;

        // loop forever
        while(1) {
        }
}
