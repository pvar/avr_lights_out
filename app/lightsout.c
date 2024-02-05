#include "lightsout.h"

uint8_t ledMatrixState[COLS][ROWS];
uint8_t gameState[COLS][ROWS];
uint8_t gammaValues[8], gameOn;

int main(void) {
        mcuInit();
        appInit();

        uint8_t sequencer = 0;

        gameOn = 1;
        createNewLevel();

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
        DDRB  &= 0b11100000;
        PORTB &= 0b11100000;

        // set all pins as outputs for driving the led matrix
        DDRD = 0b11111111;

        // scan rows
        for (row = 0; row < 5; row++) {
                // switch row after turning all columns off, to avoid ghosting
                PORTD = 0;
                PORTC = 1 << row;

                // go through steps
                for (step = 0; step < 114; step++) {
                        // switch on/off every column of selected row
                        // according to current step and brightness level
                        tmpVal = 0b00000000;
                        for (col = 0; col < 8; col++) {
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
        uint8_t row, col, exit = 0;

        // set first 5 pins as outputs and enable pull-up resistors
        // used for enabling/disabling switches in a row
        PORTB |= 0b00011111;
        DDRB  |= 0b00011111;

        // set all pins as inputs and enable pull-up resistors
        // used for reading state of switches in a row
        PORTD = 0b11111111;
        DDRD  = 0b00000000;

        for (row = 0; row < 5; row++) {
                // enable switches of row
                PORTB &= ~(1 << row);

                for (col = 0; col < 8; col++) {
                        if ((PINB & (1 << col)) == 0) {
                                // the switch at row ROW and column COL is pressed
                                applyPatternOn(col, row);
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

void applyPatternOn(uint8_t x, uint8_t y) {
        // TODO:
        // Apply one of three patterns around selected LED
        // modification will be applied on gameState[][]
}

void checkMainButton(void) {
        // If game has ended and button was pressed,
        // create new level in gameState[][]
}

void createNewLevel(void) {
        // Added for testing purposes!
        // Will be replaced by proper implementation.
        for (int col = 0; col < 8; col++) {
                for (int row = 0; row < 5; row++) {
                        if (col & 1)
                                gameState[col][row] = BRIGHTNESS;
                }
        }
}

void updateFrameBuffer(void) {
        // Update ledMatrixState[][] according to gameState[][]
        for (int col = 0; col < 8; col++) {
                for (int row = 0; row < 5; row++) {
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
        for (int col = 0; col < 8; col++) {
                for (int row = 0; row < 5; row++) {
                        if (gameState[col][row] > 0)
                                return;
                }
        }

        // If still here, no light is on: Well done!
        playWinningTune();
        gameOn = 0;
}

void playWinningTune(void) {
}

void testLedMatrix(void) {
        for (int row = 0; row < 5; row++) {
                PORTC = 1 << row;
                for (int col = 0; col < 8; col++) {
                        PORTD = 1 << col;
                        _delay_ms(128);
                }
        }
        PORTC = 0;
}

void mcuInit (void) {
        // speaker and led matrix rows (through transistor array)
        DDRC = 0b00111111;
        // mode indicator, main button and button matrix rows
        DDRB  = 0b11000000;
        PORTB = 0b11100000;
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

        for (int col = 0; col < COLS; col++) {
                for (int row = 0; row < ROWS; row++) {
                        ledMatrixState[col][row] = 0;
                        gameState[col][row] = 0;
                }
        }
}
