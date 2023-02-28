#include "HC595.h"


void shiftInit() {
    // Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
    HC595_DDR |= ((1 << HC595_SH_CP_POS) | (1 << HC595_ST_CP_POS) | (1 << HC595_DS_POS));

    // Pins to low
    HC595_PORT &= ~((1 << HC595_SH_CP_POS) | (1 << HC595_ST_CP_POS) | (1 << HC595_DS_POS));
}


// Sends a clock pulse on SH_CP line
inline static void shiftPulse() {
    // Pulse the Shift Clock
    HC595_PORT |= (1 << HC595_SH_CP_POS);     // HIGH
    HC595_PORT &= (~(1 << HC595_SH_CP_POS));  // LOW
}


// Sends a clock pulse on ST_CP line
inline static void shiftLatch() {
    // Pulse the Store Clock
    HC595_PORT |= (1 << HC595_ST_CP_POS);  // HIGH
    _delay_loop_1(1);
    HC595_PORT &= (~(1 << HC595_ST_CP_POS));  // LOW
    _delay_loop_1(1);
}


void shiftWrite(uint16_t data) {
    for (uint8_t i = 0; i < 16; i++) {
        if (data & BIT15) {
            HC595DataHigh();
        } else {
            HC595DataLow();
        }
        shiftPulse();      // Pulse the Clock line
        data = data << 1;  // Now bring next bit at MSB position
    }
    // Now all 16 bits have been transferred to shift register
    // Move them to output. Latch once
    shiftLatch();
}
