#include <util/delay.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void mcu_init (void) {
        // speaker and led matrix rows (through transistor array)
        DDRC = 0b00111111;
        // button matrix rows, mode leds and main button
        DDRB = 0b11011111;
        // this will be changing, depending on wether
        // we read the buttons or drive the leds
        DDRD = 0b11111111;

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

int main(void) {
        mcu_init();
}
