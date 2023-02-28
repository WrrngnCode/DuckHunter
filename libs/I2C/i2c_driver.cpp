#include "i2c_driver.h"

#define TWI_TIMEOUT 1600
#define TWI_START 0x08
#define TWI_RESTART 0x10
#define TWIT_ADDR_ACK 0x18
#define TWIT_ADDR_NACK 0x20
#define TWIT_DATA_ACK 0x28
#define TWIT_DATA_NACK 0x30

#define TWIR_ADDR_ACK 0x40
#define TWIR_ADDR_NACK 0x48
#define TWIR_DATA_ACK 0x50
#define TWIR_DATA_NACK 0x58

#define TWI_ERROR 0x38
#define TWI_NONE 0xF8

#define TW_STATUS_MASK ((1 << TWS7) | (1 << TWS6) | (1 << TWS5) | (1 << TWS4) | (1 << TWS3))
#define TW_STATUS (TWSR & TW_STATUS_MASK)  // value of TWSR register without prescaler bits.

#define IS_TWINT_SET (TWCR & (1 << TWINT))

volatile static uint8_t twi_status = TWI_NONE;

static void twi_reinitialize() {
    TWCR = 0;
    _delay_loop_1(5);
    TWCR = (1 << TWEN) | (1 << TWIE);  // enable TWI , enable ISR
}


ISR(TWI_vect) {
    twi_status = TW_STATUS;
}


uint8_t get_twi_status() {
    return twi_status;
}


void twi_init_master_transmitter(void)  // Function to initialize master
{
    if ((TWCR & (1 << TWEN)) == 0) {
        DDRC |= (1 << PORTC5) | (1 << PORTC4);   // SCL and SDA as output
        PORTC |= (1 << PORTC5) | (1 << PORTC4);  // activate pull-up resistor

        TWBR = 72;  // Bit rate       // TWBR =  (F_CPU /(TWI_FREQ) -16 ) / 2

        TWSR &= ~((1 << TWPS1) | (1 << TWPS0));  // Clear prescalar bits (Prescaler = 1)
        TWCR = (1 << TWEN) | (1 << TWIE);        // enable TWI , enable ISR
    }
}


static TWI_STATE TWI_send_start_condition() {
    /*
    If the I-bit in SREG and TWIE in TWCR are set,
    the MCU will jump to the TWI Interrupt Vector.
    While the TWINT Flag is set, the SCL low period is stretched.
    The TWINT Flag must be cleared by
    software by writing a logic one to it.
    The TWI n will not start any operation as long as the TWINT bit in TWCRn is set.
    Immediately after the application has cleared TWINT, the TWI n will
    initiate transmission of the START condition.
    */
    uint16_t counter = 0;

    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
    // when transmitted TWINT flag is set, wait for flag to be set
    // TWSRn will be 0x08 when Start condition has been successfully transmitted
    // while (!IS_TWINT_SET);    // loop until TWINT is set

    while (twi_status != TWI_START) {
        ++counter;
        if (counter > TWI_TIMEOUT) {
            return TWI_STATE::TWI_ERROR_START;
        }
    }
    return TWI_STATE::TWI_OK;
}


static void twi_send_stop_condition() {
    // Note that TWINT is NOT set after a STOP condition has been sent.
    uint16_t counter = 0;
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE);
    while (TWCR & (1 << TWSTO)) {
        ++counter;
        if (counter > TWI_TIMEOUT) {
            twi_reinitialize();
            return;
        }
    }
}

static TWI_STATE TWI_address_write_ack() {
    uint16_t counter = 0;
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
    while (twi_status != TWIT_ADDR_ACK) {
        ++counter;
        if (counter > TWI_TIMEOUT) {
            return TWI_STATE::TWI_NACK;
        }
    }
    return TWI_STATE::TWI_OK;
}

static TWI_STATE TWI_data_write_ack() {
    uint16_t counter = 0;
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
    while (twi_status != TWIT_DATA_ACK) {
        ++counter;
        if (counter > TWI_TIMEOUT) {
            return TWI_STATE::TWI_NACK;
        }
    }
    return TWI_STATE::TWI_OK;
}



static TWI_STATE twi_write(uint8_t device_address,
                    uint8_t reg,
                    uint8_t* data,
                    uint8_t length) {
    uint8_t i = 0;

    TWI_STATE ret = TWI_STATE::TWI_OK;

    ret = TWI_send_start_condition();
    if (ret != TWI_STATE::TWI_OK) {
        twi_send_stop_condition();
        return ret;
    }

    TWDR = ((device_address << 1) | (0 << 0));  // 7 bit address + 0=Write

    ret = TWI_address_write_ack();
    if (ret != TWI_STATE::TWI_OK) {
        twi_send_stop_condition();
        return ret;
    }

    TWDR = reg;
    ret = TWI_data_write_ack();
    if (ret != TWI_STATE::TWI_OK) {
        twi_send_stop_condition();
        return ret;
    }

    if (length != 0 && data != 0) {
        for (i = 0; i < length; ++i) {
            TWDR = *(data + i);
            ret = TWI_data_write_ack();
            if (ret != TWI_STATE::TWI_OK) {
                twi_send_stop_condition();
                return ret;
            }
        }
    }
    twi_send_stop_condition();
    return ret;
}


TWI_STATE twi_write8bit(uint8_t device_address,
                        uint8_t data) {
    TWI_STATE ret;
    ret = twi_write(device_address, data, NULL, 0);
    if (ret != TWI_STATE::TWI_OK) {
        twi_reinitialize();
    }
    return ret;
}
