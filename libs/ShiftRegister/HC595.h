
#ifndef HC595_H_
#define HC595_H_

#include <avr/io.h>
#include <util/delay.h>

#define HC595_PORT PORTD
#define HC595_DDR DDRD
#define HC595_DS_POS PORTD4     // Data pin (DS) pin location
#define HC595_ST_CP_POS PORTD5  // Store Clock (ST_CP) pin location
#define HC595_SH_CP_POS PORTD6  // Shift Clock (SH_CP) pin location

// change data (DS)lines
#define HC595DataHigh() (HC595_PORT |= (1 << HC595_DS_POS))
#define HC595DataLow() (HC595_PORT &= (~(1 << HC595_DS_POS)))

#define BIT15 (1<<15)

void shiftInit();
void shiftWrite(uint16_t data);

#endif