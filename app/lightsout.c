#include "lightsout.h"

uint8_t currentState[COLS][ROWS];
uint8_t targetState[COLS][ROWS];

int main(void) {
        mcuInit();
        appInit();

        while(1) {
                ledMatrixPWM();
        }
}

void ledMatrixPWM(void) {
        uint8_t row, col, step, tmpVal;

        // set PORTD as output for driving the led matrix
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
        DDRB = 0b11000000;
        PORTB = 0b10000000;
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
        for (int col = 0; col < COLS; col++) {
                for (int row = 0; row < ROWS; row++) {
                        currentState[col][row] = 0;
                        targetState[col][row] = 0;
                }
        }
}
