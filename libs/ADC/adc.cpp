#include "adc.h"

#define ADC_PRESCALER_128 ((1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2))
#define ADC_TRIGGER_TIMER0_COMPARE_MATCH_A ((1 << ADTS1) | (1 << ADTS0))
#define ADC_TRIGGER_TIMER1_OVF ((1 << ADTS2) | (1 << ADTS1))
#define ADC_TRIGGER_TIMER1_COMPARE_MATCH_B ((1 << ADTS2) | (1 << ADTS0))


void AnalogDigitalConverter::initADC5() {
    ADMUX |= (1 << REFS0);  // AVCC as voltage reference
    ADMUX |= INPUT_CHANNEL_ADC5;
    ADCSRA |= ((1 << ADEN) | (1 << ADIE));  // Turn on ADC, Enable ADC Completed Interrupt
    ADCSRA |= ADC_PRESCALER_128;
    // Disable digital input buffer
    DIDR0 = (1 << ADC5D);
}

void AnalogDigitalConverter::initADC0() {
    ADMUX |= (1 << REFS0);  // AVCC as voltage reference
    ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
    ADCSRA |= ((1 << ADEN) | (1 << ADIE));  // Turn on ADC, Enable ADC Completed Interrupt
    ADCSRA |= ADC_PRESCALER_128;
    // Disable digital input buffer
    DIDR0 = (1 << ADC0D);
}

void AnalogDigitalConverter::initADC0_AutoTrigger() {
    ADMUX |= (1 << REFS0);  // AVCC as voltage reference
    // MUX[3:0] = 0 => ADC0
    ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
    ADCSRA |= ((1 << ADATE));               // Auto Trigger Enable
    ADCSRA |= ((1 << ADEN) | (1 << ADIE));  // Turn on ADC, Enable ADC Completed Interrupt
    ADCSRA |= ADC_PRESCALER_128;
    ADCSRB = 0;  // Free Running Mode
    // Disable digital input buffer
    DIDR0 = (1 << ADC0D);
    startConversion();
}


void AnalogDigitalConverter::initADC0_TriggerTimer0OCA() {
    initADC_TriggerTimer0OCA(INPUT_CHANNEL_ADC0);
}


void AnalogDigitalConverter::initADC_TriggerTimer0OCA(uint8_t mux_channel) {
    ADMUX |= (1 << REFS0);  // AVCC as voltage reference

    ADCSRB = 0;
    ADCSRB |= ADC_TRIGGER_TIMER0_COMPARE_MATCH_A; // select auto trigger source Timer/Counter0 compare match A

    mux_channel &= ((1 << MUX2) | (1 << MUX1) | (1 << MUX0)); // allow only channels ADC0-ADC7
    ADMUX |= mux_channel;

    ADCSRA |= ((1 << ADATE));               // Auto Trigger Enable
    ADCSRA |= ((1 << ADEN) | (1 << ADIE));  // Turn on ADC, Enable ADC Completed Interrupt

    // Disable digital input buffer
    DIDR0 = (1 << ADC0D);

    ADCSRA |= ADC_PRESCALER_128;
    startConversion();
}


void AnalogDigitalConverter::startConversion() {
    ADCSRA |= (1 << ADSC);
}