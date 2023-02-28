#include "InputComponent.h"


extern volatile uint32_t sysTick;

#define POTMETER_SAMPLE_TIMETICKS (10U)  // Sampling period: 10 ticks (~16ms/tick)
#define DELTA_POT_MAX ((int32_t)600)     // upper limit potmeter rotation in 10 ticks
#define DELTA_POT_MIN ((int32_t)0)       // lower limit potmeter rotation in 10 ticks
#define DELTA_POT_THRESHOLD_LOW ((int8_t)(DELTA_POT_MAX * -0.08))
#define DELTA_POT_THRESHOLD_HIGH ((int8_t)(DELTA_POT_MAX * 0.08))
#define TICK_DELAY_FAST ((int32_t)3)   // frame update delay in ticks, lower limit (fast update)
#define TICK_DELAY_SLOW ((int32_t)35)  // frame update delay in ticks, upper limit (slow update)


InputComponent::InputComponent(volatile uint16_t* ptr_adc)
    : potmeter(ptr_adc) {
}

uint8_t InputComponent::calc_ticks_per_step(int32_t deltaPot) {
    // Absolute value
    deltaPot = deltaPot < 0 ? (-1) * deltaPot : deltaPot;
    if (deltaPot > DELTA_POT_MAX) deltaPot = DELTA_POT_MAX;
    // map deltaPot to a value between TICK_DELAY_FAST and TICK_DELAY_SLOW
    // lower deltaPot -> higher frame update delay (inverse mapping)
    return (deltaPot - DELTA_POT_MIN) *
               (TICK_DELAY_FAST - TICK_DELAY_SLOW) /
               (DELTA_POT_MAX - DELTA_POT_MIN) +
           TICK_DELAY_SLOW;
}

// Process Inputs from Potentiometer
void InputComponent::update(Alien* alien) {
    static uint16_t lastTime = 0;
    static uint32_t lastSample = 0;

    volatile uint16_t currentSample = *potmeter;

    if (lastTime == 0) {  // first time, syncronize
        lastTime = sysTick;
        lastSample = currentSample;
        return;
    }

    if (sysTick - lastTime >= POTMETER_SAMPLE_TIMETICKS) {
        int32_t deltaPot = (int32_t)currentSample - (int32_t)lastSample;
        if (deltaPot > DELTA_POT_THRESHOLD_HIGH || deltaPot < DELTA_POT_THRESHOLD_LOW) {
            alien->spd_ticks_per_step = calc_ticks_per_step(deltaPot);

            if (deltaPot > 0) {
                alien->setState(MOVING_RIGHT);
            } else {
                alien->setState(MOVING_LEFT);
            }
        }
        lastTime = sysTick;
        lastSample = currentSample;
    }
}
