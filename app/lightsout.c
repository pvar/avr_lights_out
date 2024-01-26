#include "lightsout.h"

// Frame buffers for the LED matrix
uint8_t currentState[COLS][ROWS];
uint8_t targetState[COLS][ROWS];

int main(void) {
        mcu_init();
        app_init();

        while(1) {
                testLedMatrix();
        }
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

void mcu_init (void) {
        // speaker and led matrix rows (through transistor array)
        DDRC = 0b00111111;
        // mode indicator, main button and button matrix rows
        DDRB = 0b11011111;
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

void app_init (void) {
        for (int x = 0; x < COLS; x++) {
                for (int y = 0; y < ROWS; y++) {
                        currentState[x][y] = 0;
                        targetState[x][y] = 0;
                }
        }
}
