
#ifndef INPUTCOMPONENT_H_
#define INPUTCOMPONENT_H_

#include <stdint.h>
#include "Alien.h"

class InputComponent {
   public:
    void update(Alien* alien);
    InputComponent(volatile uint16_t* ptr_adc);

   private:
    volatile uint16_t* potmeter;
    static uint8_t calc_ticks_per_step(int32_t deltaPot);
};


#endif