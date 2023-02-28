#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>

#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a) ((a)*clockCyclesPerMicrosecond())

// prescaler 1024
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(1024L * 256L))
#define MS_TO_TICKS(ms) ((ms)*1000L / MICROSECONDS_PER_TIMER0_OVERFLOW)

class Timer0 {
   private:
    uint8_t m_duty_cycle = 50;

   public:
    void initT0_NormalMode(uint8_t compareA);
    void initT0_CTC();
    void initT0_PWM(uint8_t duty_cycle);
    void incrementDutyCycleT0(uint8_t duty_cycle);
    uint8_t getDutyCycle();
};

// 16 bit Timer
class Timer1 {
   public:
    void initT1_CTC();
    void initT1_PWM();
    void initT1_1sec(uint16_t compareA, uint16_t compareB);
    void initT1_PWM_OC1A_OC1B(uint8_t freqA, uint8_t freqB);
};

class Timer2 {
   public:
    void initT2_PWM_OC2A(uint8_t freqA);
};

#endif