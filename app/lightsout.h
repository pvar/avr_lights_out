#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void mcuInit(void);
void appInit(void);
void clearGame(void);
void testLedMatrix(void);
void updateLedMatrix(void);
void scanSwitchMatrix(void);
void applyPatternOnMatrix(uint8_t x, uint8_t y);
void applyPatternOnRow(uint8_t x, uint8_t y, uint8_t pattern);
void checkMainButton(void);
void createNewLevel(void);
void updateFrameBuffer(void);
void checkGameState(void);
void playVictoryTune(void);
void playStartTune(void);
void playRestartTune(void);
void playInitTune(void);

#define COLS 8
#define ROWS 5
#define SPK_PIN 5
#define BTN_PIN 5
#define BRIGHTNESS 4
#define DEBOUNCE 6
/*
MATRIX SCANNING TIMINGS:

        FRAME:
                frame rate: 125Hz
                time for frame: 8ms
                time fow line: 1.6ms

        LINE:
                steps: 114
                time per step: 14us
                step frequency: ~71.25kHz
                gamma corrected levels: 0, 1, 2, 4, 6, 9, 16, 18

GAME PATTERNS:

        PATTERN 1
                patternAbove[0] = 0b00000010        -  T  -
                patternThis[0]  = 0b00000111        T  T  T
                patternBelow[0] = 0b00000010        -  T  -

        PATTERN 2
                patternAbove[1] = 0b00000101        T  -  T
                patternThis[1]  = 0b00000010        -  T  -
                patternBelow[1] = 0b00000101        T  -  T

        PATTERN 3
                patternAbove[2] = 0b00000101        T  -  T
                patternThis[2]  = 0b00000010        -  T  -
                patternBelow[2] = 0b00000011        -  T  T

DIFFICULTY:

        EASY:  mode = 0
        HARD:  mode = 1
        DEVIL: mode = 2
*/
