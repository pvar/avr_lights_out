#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void mcuInit (void);
void appInit (void);
void testLedMatrix(void);
void ledMatrixPWM(void);

#define COLS 8
#define ROWS 5

#define SPK_PIN 5
#define BTN_PIN 5

/*

MATRIX SCANNING TIMINGS:

        FRAME:
                frame rate: 125Hz
                time for frame: 8ms
                time fow line: 1.6ms

        LINE:
                steps: 228
                time per step: ~7us
                step frequency: ~142.5kHz
                gamma corrected levels: 0, 1, 2, 4, 6, 9

*/
