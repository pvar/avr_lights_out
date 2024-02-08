#include "lightsout.h"

uint8_t ledMatrixState[COLS][ROWS];
uint8_t gameState[COLS][ROWS];
uint8_t gammaValues[8];
uint8_t patternAbove[3];
uint8_t patternBelow[3];
uint8_t patternThis[3];
uint8_t mode, gameOn;

int main(void) {
        mcuInit();
        appInit();

        uint8_t sequencer = 0;

        while(1) {
                sequencer++;
                updateLedMatrix();
                switch (sequencer) {
                case 1:
                        scanSwitchMatrix();
                        break;
                case 2:
                        checkMainButton();
                        break;
                case 3:
                        updateFrameBuffer();
                        break;
                case 4:
                        checkGameState();
                        sequencer = 0;
                }
        }
}

void updateLedMatrix(void) {
        uint8_t row, col, step, tmpVal;

        // set first 5 pins to Hi-z state
        PORTB &= 0b11100000;
        DDRB  &= 0b11000000;

        // set all pins as outputs for driving the led matrix
        DDRD = 0b11111111;

        // scan rows
        for (row = 0; row < ROWS; row++) {
                // switch row after turning all columns off, to avoid ghosting
                PORTD = 0;
                PORTC = 1 << row;

                // go through steps
                for (step = 0; step < 114; step++) {
                        // switch on/off every column of selected row
                        // according to current step and brightness level
                        tmpVal = 0b00000000;
                        for (col = 0; col < COLS; col++) {
                                if (step < gammaValues[ledMatrixState[col][row]]) {
                                        tmpVal |= 1 << col;
                                }
                        }
                        PORTD = tmpVal;

                        // assuming that each iteration takes 8us,
                        // we add 6us to reach desired period (~14us)
                        _delay_us(6);
                }
        }

        // we only spent 6us for the last step of the last row
        // so, we have to add 8us to reach desired period (~14us)
        _delay_us(8);
        PORTC = 0;
}

void scanSwitchMatrix(void) {
        static uint8_t deBounceDelay = 0;

        if (deBounceDelay > 0) {
                deBounceDelay--;
                return;
        }

        // set first 5 pins as outputs and enable pull-up resistors
        // used for enabling/disabling switches in a row
        PORTB |= 0b00111111;
        DDRB  |= 0b00011111;

        // set all pins as inputs and enable pull-up resistors
        // used for reading state of switches in a row
        PORTD = 0b11111111;
        DDRD  = 0b00000000;

        uint8_t row, col, exit = 0;
        for (row = 0; row < ROWS; row++) {
                // enable switches of row
                PORTB &= ~(1 << row);
                _delay_us(1);

                for (col = 0; col < COLS; col++) {
                        if ((PIND & (1 << col)) == 0) {
                                // the switch at row 'row' and column 'col' is pressed
                                applyPatternOnMatrix(col, row);
                                deBounceDelay = DEBOUNCE;
                                // ignore any other pressed switches for now
                                exit = 1;
                                break;
                        }
                }

                // disable switches of row
                PORTB |= 1 << row;

                if (exit)
                    break;
        }
}

void applyPatternOnMatrix(uint8_t x, uint8_t y) {

        // apply on same row
        applyPatternOnRow(x, y, patternThis[mode]);

        // row 0 is the top most
        if (y > 0) {
                // apply on row above
                applyPatternOnRow(x, y - 1, patternAbove[mode]);
        }

        // row 5 (index 4) is the lower most
        if (y < 4) {
                // apply on row below
                applyPatternOnRow(x, y + 1, patternBelow[mode]);
        }
}

void applyPatternOnRow(uint8_t x, uint8_t y, uint8_t pattern) {
        // bit 0 is the least significant -- left-most side
        if  (x > 0) {
                // check previous bit
                if (pattern & 0b00000100)
                        gameState[x-1][y] = BRIGHTNESS - gameState[x-1][y];
        }

        // bit 7 is the most significant -- right-most side
        if  (x < 6) {
                // check next bit
                if (pattern & 0b00000001)
                        gameState[x+1][y] = BRIGHTNESS - gameState[x+1][y];
        }

        // check this bit
        if (pattern & 0b00000010)
                gameState[x][y] = BRIGHTNESS - gameState[x][y];
}

void checkMainButton(void) {
        static uint8_t pressDuration = 0;
        static uint8_t deBounceDelay = 0;

        // setup button pin
        // this is probably already taken care of, but you never know
        PINB  &= ~(1<<BTN_PIN);
        PORTB |=  (1<<BTN_PIN);

        if (deBounceDelay > 0) {
                deBounceDelay--;
                if (!(PINB & (1<<BTN_PIN)))
                        pressDuration++;
                return;
        }

        // check if button was pressed (pin grounded)
        if (pressDuration == 0 && !(PINB & (1<<BTN_PIN))) {
                deBounceDelay = DEBOUNCE;
                pressDuration = 1;
                return;
        }

        if (pressDuration >= DEBOUNCE) {
                // duration of key-press: long

                // start or restart game
                if (gameOn == 0)
                        playStartTune();
                else
                        playRestartTune();

                clearGame();
                createNewLevel();
                gameOn = 1;
        } else if (pressDuration > 0) {
                // duration of key-press: short

                // set game mode (difficulty)
                if (mode < 2)
                        mode++;
                else
                        mode = 0;

                // update leds
                PORTB &= 0b00111111;
                PORTB |= (mode + 1) << 6;
        }

        pressDuration = 0;
}

void createNewLevel(void) {
        // Added for testing purposes!
        // Will be replaced by proper implementation.
        for (int col = 0; col < COLS; col++) {
                for (int row = 0; row < ROWS; row++) {
                        if (col & 1)
                                gameState[col][row] = BRIGHTNESS;
                }
        }
}

void updateFrameBuffer(void) {
        // Update ledMatrixState[][] according to gameState[][]
        for (int col = 0; col < COLS; col++) {
                for (int row = 0; row < ROWS; row++) {
                        // fade in...
                        if (ledMatrixState[col][row] < gameState[col][row])
                                ledMatrixState[col][row]++;
                        // fade out...
                        if (ledMatrixState[col][row] > gameState[col][row])
                                ledMatrixState[col][row]--;
                }
        }
}

void checkGameState(void) {
        // Check if any light is on...
        for (int col = 0; col < COLS; col++) {
                for (int row = 0; row < ROWS; row++) {
                        if (gameState[col][row] > 0)
                                return;
                }
        }

        // If still here, no light is on: Well done!
        playVictoryTune();
        gameOn = 0;
}

void playVictoryTune(void) {
}

void playStartTune(void) {
}

void playRestartTune(void) {
}

void playInitTune(void) {
}


void testLedMatrix(void) {
        for (int row = 0; row < ROWS; row++) {
                PORTC = 1 << row;
                for (int col = 0; col < COLS; col++) {
                        PORTD = 1 << col;
                        _delay_ms(128);
                }
        }
        PORTC = 0;
}

void clearGame (void) {
        for (int col = 0; col < COLS; col++) {
                for (int row = 0; row < ROWS; row++) {
                        ledMatrixState[col][row] = 0;
                        gameState[col][row] = 0;
                }
        }
}

void mcuInit (void) {
        // speaker and led matrix rows (through transistor array)
        DDRC = 0b00111111;
        // mode indicator, main button and button matrix rows
        DDRB  = 0b11000000;
        PORTB = 0b01100000;
        // this will be constantly changing
        // - input for reading the button matrix
        // - output for driving the led matrix
        DDRD = 0b00000000;

        // disable analog to digital converter
        ADCSRA = ADCSRA & ~(1<<ADEN);

        // completely shutdown analog to digital converter
        PRR = PRR | (1<<PRADC);

        // turn off analog comparator
        ACSR = ACSR | (1<<ACD);
        ACSR = ACSR & ~(1<<ACBG);

        // select and enable sleep mode
        //ldi tmp, (1<<SE)
        //out MCUCR, tmp
}

void appInit (void) {
        // brightness levels for smooth fading
        gammaValues[0] = 0;
        gammaValues[1] = 1;
        gammaValues[2] = 2;
        gammaValues[3] = 4;
        gammaValues[4] = 6;
        gammaValues[5] = 9;
        gammaValues[6] = 16;
        gammaValues[7] = 28;

        patternAbove[0] = 0b00000010;
        patternThis[0]  = 0b00000111;
        patternBelow[0] = 0b00000010;

        patternAbove[1] = 0b00000101;
        patternThis[1]  = 0b00000010;
        patternBelow[1] = 0b00000101;

        patternAbove[2] = 0b00000101;
        patternThis[2]  = 0b00000010;
        patternBelow[2] = 0b00000011;

        playInitTune();

        clearGame();
        createNewLevel();

        mode = 0;
        gameOn = 1;
}
