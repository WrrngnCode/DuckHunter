#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

typedef enum { TWI_OK = 0,
               TWI_ERROR_START = 1,
               TWI_NACK = 2
} TWI_STATE;


void twi_init_master_transmitter();
TWI_STATE twi_write8bit(uint8_t device_address, uint8_t data);
uint8_t get_twi_status(); // get TWSR status register

#endif
