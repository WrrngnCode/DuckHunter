#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

#define INPUT_CHANNEL_ADC7 ((1 << MUX2) | (1 << MUX1) | (1 << MUX0))
#define INPUT_CHANNEL_ADC5 ((1 << MUX2) | (1 << MUX0))
#define INPUT_CHANNEL_ADC0 (0)


class AnalogDigitalConverter{
    public:
        void initADC_TriggerTimer0OCA(uint8_t mux_channel);
        void initADC0_AutoTrigger();
        void initADC0_TriggerTimer0OCA();
        void initADC0();
        void initADC5();
        void startConversion();
};

#endif