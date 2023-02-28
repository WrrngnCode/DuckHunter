#ifndef TM1637_H_
#define TM1637_H_

#include <avr/io.h>

// PIN DEFINITION FOR CLOCK AND DATA PIN
#define DIO_PORT PORTD
#define CLK_PORT PORTD
#define DIO_PIN PORTD3
#define CLK_PIN PORTD2
#define DIO_DDR DDRD
#define CLK_DDR DDRD

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//==============================================================================
// Definitions
//==============================================================================
//Segments
//  --0x01--
// |        |
//0x20     0x02
// |        |
//  --0x40- -
// |        |
//0x10     0x04
// |        |
//  --0x08--


//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     --- .
//      D


#define A_SEG 0x01
#define B_SEG 0x02
#define C_SEG 0x04
#define D_SEG 0x08
#define E_SEG 0x10
#define F_SEG 0x20
#define G_SEG 0x40
#define DP_SEG 0x80

#define SEG_A 0x01
#define SEG_B 0x02
#define SEG_C 0x04
#define SEG_D 0x08
#define SEG_E 0x10
#define SEG_F 0x20
#define SEG_G 0x40

#define TM1637_DOT (DPSEG)
#define TM1637_MINUS (SEG_G)
#define TM1637_BLANK (0)
#define TM1637_ZERO 0x3F  //0b0011111

// Commands
#define DATA_SET 0x40
#define DATA_FIXED 0x44
#define DISPLAY_ON 0x88
#define DISPLAY_OFF 0x80
#define PACKET_SIZE 0x08

#define TM1637_CMD_SETUP 0x40       //0b01000000    // Data command setting for setting normal mode
#define TM1637_CMD_SET_DATA 0xC0    //0b11000000    // Address command setting
#define TM1637_CMD_DISPLAY_ON 0x88  //0b10001000    // display on

/*
If DDxn is written logic one, Pxn is configured as an output
pin. If DDxn is written logic zero, Pxn is configured as an input pin.
If PORTxn is written logic one when the pin is configured as an input pin,
the pull-up resistor is activated. To switch the
pull-up resistor off, PORTxn has to be written logic zero
or the pin has to be configured as an output pin. The port pins are
tri-stated when reset condition becomes active, even if no clocks are running.

DDR=0 PORT=0   Tri-State / Hi-Z (Open)
DDR=0 PORT=1   Pull Up
DDR=1 PORT=0   Driven Low
DDR=1 PORT=1   Driven High

Since only register can be changed at a time, the pin will go through
an undesired state switching between 0-0 Tri-state to 1-1 driven high.

HIGH = Input   DDR=0 PORT=0   Tri-State (Open)
LOW  = Output  DDR=1 PORT=0   Driven Low

*/

#define CLK_INP cbi(CLK_DDR, CLK_PIN)
#define CLK_OUT sbi(CLK_DDR, CLK_PIN)

#define DIO_INP cbi(DIO_DDR, DIO_PIN)
#define DIO_OUT sbi(DIO_DDR, DIO_PIN)

#define CLK_H CLK_INP
#define CLK_L CLK_OUT

#define DIO_H DIO_INP
#define DIO_L DIO_OUT

#define HIGH 1
#define LOW 0

#define DELAY_US (13) // delay in microseconds

// Display Functions
void TM1637_init();
void TM1637_clear(void);
void TM1637_setBrightness(uint8_t brightness);
void TM1637_setSegmentBuffer(const uint8_t* buffer);
void TM1637_displaySegments(uint8_t length, uint8_t screenPos);
void TM1637_displayDigits(const uint8_t* digits, uint8_t length, uint8_t screenPos);
uint8_t TM1637_displayNumber(int32_t num, uint8_t base);

#endif

