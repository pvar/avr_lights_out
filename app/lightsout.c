#include "lightsout.h"

uint8_t currentState[COLS][ROWS];
uint8_t targetState[COLS][ROWS];
uint8_t gammaValues[5];

int main(void) {
        mcuInit();
        appInit();

        uint8_t sequencer = 0;

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
                for (step = 0; step < 228; step++) {
                        // switch on/off every column of selected row
                        // according to current step and brightness level
                        tmpVal = 0b00000000;
                        for (col = 0; col < 8; col++) {
                                if (step < currentState[col][row]) {
                                        tmpVal |= 1 << col;
                                }
                        }
                        PORTD = tmpVal;

                        // assuming that each iteration takes 5us,
                        // we add 2us to reach desired period (~7us)
                        _delay_us(2);
                }
        }

        // we only had 2us, for the last step of the last row
        // so, we have to add 5us to reach desired period (~7us)
        _delay_us(5);
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
        PORTB = 0b11111111;
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
        // modification will be applied on targetState[][]
}

void checkMainButton(void) {
        // If game has ended and button was pressed,
        // create new level in targetState[][]
}

void createNewLevel(void) {
        // Added for testing purposes!
        // Will be replaced by proper implementation.
        for (int col = 0; col < 8; col++) {
                for (int row = 0; row < 5; row++) {
                        if (col & 1)
                                targetState[col][row] = BRIGHTNESS;
                }
        }
}

void updateFrameBuffer(void) {
        // Update currentState[][] according to targetState[][]
        for (int col = 0; col < 8; col++) {
                for (int row = 0; row < 5; row++) {
                        // fade in...
                        if (currentState[col][row] < targetState[col][row])
                                currentState[col][row]++;
                        // fade out...
                        if (currentState[col][row] > targetState[col][row])
                                currentState[col][row]--;
                }
        }
}

void checkGameState(void) {
        // Check if game has ended -- all LEDS out!
        // Play sound if user won
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

        for (int col = 0; col < COLS; col++) {
                for (int row = 0; row < ROWS; row++) {
                        currentState[col][row] = 0;
                        targetState[col][row] = 0;
                }
        }
}
