
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "Game.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "Timer.h"
#include "adc.h"
#include "init.h"
#include "uart.h"


void* operator new(size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete(void* ptr, unsigned int size) noexcept {
    (void)size;
    free(ptr);
}

char message[40];
volatile uint32_t sysTick = 0;
volatile uint16_t adc_value;

UART uart;
static AnalogDigitalConverter adc;

Game* CreateGame() {
    return new Game(new InputComponent(&adc_value),
                    new GraphicsComponent(),
                    new PhysicsComponent());
}

int main(void) {

#if ADC_CHANNEL == 7
    adc.initADC_TriggerTimer0OCA(INPUT_CHANNEL_ADC7);
#elif ADC_CHANNEL == 0
    adc.initADC_TriggerTimer0OCA(INPUT_CHANNEL_ADC0);
#endif

    init_pins();

    Timer0 timer0;
    Timer1 timer1;
    Timer2 timer2;
    timer0.initT0_NormalMode(255);
    timer1.initT1_PWM_OC1A_OC1B(4, 4);  // Set PWM for PB1, PB2
    timer2.initT2_PWM_OC2A(4);          // Set PWM for PB3

    _delay_ms(10);

    sei();  // enable interrupts

    uart.init();
    uart.writeTransmitBuffer("Start\r\n");

    Game* game = CreateGame();
    game->init();

    while (1) {
        game->update();
    }
}
