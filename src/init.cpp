#include <avr/io.h>
#include "init.h"


void init_pins() {
    DDRB |= (1 << DDB5);  // PB5 (pin 13 = output)
    DDRB |= (1 << DDB4);  // PB4 (pin 12 = output)

    // Configure PWM Pins as output
    DDRB |= (1 << DDB1);  // OC1A=PB1 (pin  9 = output)
    DDRB |= (1 << DDB2);  // OC1B=PB2 (pin 10 = output)
    DDRB |= (1 << DDB3);  // OC2A=PB3 (pin 11 = output)

    // Clear PWM Pins
    PORTB &= ~((1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3));

    // Interrupt for A0 Pushbutton PORTC0 (PCINT8)
    DDRC &= ~(1 << PORTC0);  // PORTC0 (A0) = input (PCINT8)
    PORTC |= (1 << PORTC0);  // Enable pull up resistor
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT8);
}
