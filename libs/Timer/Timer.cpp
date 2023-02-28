
#include "Timer.h"
#include "avr/interrupt.h"

#define CTC_MODE (1 << WGM01)
#define PWM_MODE ((1 << WGM00) | (1 << WGM01))
#define PWM_NON_INVERTING_MODEA (1 << COM0A1)
#define PWM_INVERTING_MODEA ((1 << COM0A1) | (1 << COM0A0))
#define PWM_NON_INVERTING_MODEB (1 << COM0B1)
#define PWM_INVERTING_MODEB ((1 << COM0B1) | (1 << COM0B0))
#define CLOCK_SELECT_256 (1 << CS02)
#define CLOCK_SELECT_1024 ((1 << CS02) | (1 << CS00))
#define TIMER1_FastPWM_TCR1_TOP_MODE14A (1 << WGM11)
#define TIMER1_FastPWM_TCR1_TOP_MODE14B ((1 << WGM12) | (1 << WGM13))
#define TIMER1_PRESCALER_256 (1 << CS12)
#define TIMER1_PRESCALER_1024 ((1 << CS12) | (1 << CS10))
#define TIMER2_PRESCALER_1024 ((1 << CS22) | (1 << CS21) | (1 << CS20))

void Timer0::initT0_CTC() {
    // TCCR0A |= (1<< COM0A0) | (1<<COM0A1); //set OC0A on compare match
    TCCR0A |= (1 << COM0A0);            // toggle OC0A on compare match
    TCCR0A |= (1 << COM0B0);            // toggle OC0B on compare match
    DDRD |= (1 << DDD5) | (1 << DDD6);  // set OC0A and OC0B as output
    TCCR0A |= CTC_MODE;                 // set CTC bit
    /*
    NOTE:
    OCR0A Register is used to manipulate the counter resolution:
    the counter is cleared to ZERO when the counter value (TCNT0)
    matches the OCR0A (CTC mode).
    */
    OCR0A = 255;
    OCR0B = 101;
    TIMSK0 |= (1 << OCIE0A);
    TIMSK0 |= (1 << OCIE0B);
    TIMSK0 |= (1 << TOIE0);
    TCCR0B |= CLOCK_SELECT_1024;  // set prescaler to start the timer.
}

void Timer0::initT0_PWM(uint8_t duty_cycle) {
    DDRD |= (1 << DDD6);  // set OC0A as output
    DDRD |= (1 << DDD5);  // set OC0B as output
    TCCR0A |= PWM_MODE;
    // Clear OC0A on Compare Match, set OC0A at BOTTOM (non-inverting mode)
    TCCR0A |= PWM_NON_INVERTING_MODEA;
    // Clear OC0B on Compare Match, set OC0B at BOTTOM (non-inverting mode)
    TCCR0A |= PWM_NON_INVERTING_MODEB;

    TIMSK0 |= (1 << OCIE0A);
    TIMSK0 |= (1 << OCIE0B);
    TIMSK0 |= (1 << TOIE0);
    m_duty_cycle = duty_cycle / 100.0 * 255;
    OCR0A = m_duty_cycle;
    OCR0B = m_duty_cycle + 30 / 100.0 * 255;
    TCCR0B |= CLOCK_SELECT_1024;
}

void Timer0::initT0_NormalMode(uint8_t compareA) {
    // Normal Mode
    TCCR0A = 0;
    TCCR0B &= ~(1 << WGM02);

    if (compareA != 0) {
        OCR0A = compareA;
        TIMSK0 |= (1 << OCIE0A);
    }

    TIMSK0 |= (1 << TOIE0);
    TCCR0B |= CLOCK_SELECT_1024;  // set prescaler to start the timer.
}

uint8_t Timer0::getDutyCycle() {
    return m_duty_cycle;
}

void Timer0::incrementDutyCycleT0(uint8_t increment) {
    m_duty_cycle += increment / 100.0 * 255;
    OCR0A = m_duty_cycle;
    OCR0B = m_duty_cycle + 20 / 100.0 * 255;
}

void Timer1::initT1_CTC() {
    DDRB |= (1 << DDB1) | (1 << DDB2);  // set OC1A (PB1) and OC1B (PB2) as output
    TCCR1A |= (1 << WGM12);             // CTC mode
    OCR1A = 12200;
    TCCR1A |= (1 << COM1A0);  // toggle OC1A, NON PWM Mode
    TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);  // prescaler 8
    TCCR1B |= (1 << WGM12);                             // CTC mode TOP=OCR1A
    TIMSK1 |= (1 << TOIE1);
}

void Timer1::initT1_PWM() {
    // Clear OC1A and OC1B on Compare Match / Set OC1A and OC1B at Bottom;
    DDRB |= (1 << PB1) | (1 << PB2);
    ICR1 = 15999;
    OCR1A = 3999;
    OCR1B = 11999;
    TIMSK1 |= (1 << TOIE1);
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    // TCCR1B |= (1 << CS12);  // prescaler 256
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // prescaler = 1 (none)
}

void Timer1::initT1_PWM_OC1A_OC1B(uint8_t compareA, uint8_t compareB) {

    // Wave Form Generator: Fast PWM 14, Top = ICR1
    DDRB |= (1 << PB1) | (1 << PB2);
    ICR1 = 255;
    OCR1A = compareA;
    OCR1B = compareB;
    TIMSK1 |= (1 << TOIE1);

    // Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode)
    TCCR1A = (1 << COM1A1) | (1 << COM1B1);

    TCCR1A |= TIMER1_FastPWM_TCR1_TOP_MODE14A;
    TCCR1B |= TIMER1_FastPWM_TCR1_TOP_MODE14B;
    TCCR1B |= TIMER1_PRESCALER_1024;
}

void Timer1::initT1_1sec(uint16_t compareA, uint16_t compareB) {
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    TCCR1A |= TIMER1_FastPWM_TCR1_TOP_MODE14A;
    TCCR1B |= TIMER1_FastPWM_TCR1_TOP_MODE14B;
    ICR1 = 62499;
    if (compareA != 0) {
        OCR1A = compareA;
        TIMSK1 |= (1 << OCIE1A);
    }
    if (compareB != 0) {
        OCR1B = compareB;
        TIMSK1 |= (1 << OCIE1B);
    }
    TIMSK1 |= (1 << TOIE1);  // Enable overflow interrupt
    TCCR1B |= TIMER1_PRESCALER_256;
}

void Timer2::initT2_PWM_OC2A(uint8_t compareA) {
    // Clear OC2A on Compare Match / Set OC2A at Bottom

    DDRB |= (1 << PB3);

    // duty cycle
    OCR2A = compareA;

    // Fast PWM Mode
    TCCR2A |= (1 << WGM20) | (1 << WGM21);

    // Clear OC2A on Compare Match, set OC2A at BOTTOM (non-inverting mode)
    TCCR2A |= (1 << COM2A1);

    // Set Prescaler 1024
    TCCR2B |= TIMER2_PRESCALER_1024;
}
