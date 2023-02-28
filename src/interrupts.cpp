
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include "adc.h"
#include "uart.h"
#include "utils.h"

extern char message[40];
extern UART uart;

extern volatile uint32_t sysTick;
extern volatile uint16_t adc_value;

ISR(USART_TX_vect) {
    uart.transmitISR();
}

ISR(TIMER0_OVF_vect) {
    ++sysTick;  // (1024UL * 256UL) / 16000UL = 16 milliseconds/sysTick;
}

ISR(ADC_vect) {
    adc_value = ADC;
}

ISR(PCINT1_vect) {
    static volatile uint32_t lastPressed = 0;
    if ((sysTick - lastPressed > 5)) {  // ~5*16ms
        if (adc_value > 500) {
            OCR1A = (OCR1A + 3) & 0xFF;
            OCR1B = (OCR1B + 3) & 0xFF;
            OCR2A = (OCR2A + 3) & 0xFF;
        } else {
            if (OCR1A > 4) {
                OCR1A -= 3;
                OCR1B -= 3;
                OCR2A -= 3;
            }
        }
        lastPressed = sysTick;
    }
}

ISR(TIMER0_COMPA_vect) {
    // triggers ADC conversion in the background
}

// ISR(TIMER1_COMPB_vect) {
//     // not used
// }

ISR(TIMER1_OVF_vect) {
    // not used
}
