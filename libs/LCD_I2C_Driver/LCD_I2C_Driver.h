#ifndef LCD_I2C_DRIVER_H_
#define LCD_I2C_DRIVER_H_

#include "i2c_driver.h"

#ifndef LCD_ADDRESS
#define LCD_ADDRESS 0x27
#endif

//#define LCD_ADDRESS_WRITE ((LCD_ADDRESS << 1) | (0 << 0))  //Write = low level
//#define LCD_ADDRESS_READ ((LCD_ADDRESS << 1) | (1 << 0))   //Read = high level
#define DB0 0
#define DB1 1
#define DB2 2
#define DB3 3
#define DB4 4
#define DB5 5
#define DB6 6
#define DB7 7


#define CMD_LCD_CLEAR ((1 << DB0))
#define CMD_LCD_RETURN_HOME ((1 << DB1))                                        // Sets DDRAM address 0 in address counter.
#define CMD_LCD_CONTROL_ON ((1 << DB3) | (1 << DB2) | (1 << DB1) | (1 << DB0))  // Display on, cursor on and blinking
#define CMD_LCD_CONTROL_ON_CURSOR_OFF ((1 << DB3) | (1 << DB2))                 // Display on, cursor off
#define CMD_LCD_CONTROL_ON_NOBLINK ((1 << DB3) | (1 << DB2) | (1 << DB1))       // Display on, cursor on and not blinking
#define CMD_LCD_CONTROL_OFF ((1 << DB3))                                        // Display off, cursor off
#define CMD_LCD_SET_4BIT_2LINE ((1 << DB3) | (1 << DB5))                        // 0b00101000  // 5*8 dots (Bit 2=0)
#define CMD_LCD_SET_SECOND_LINE ((1 << DB7) | (1 << DB6))
#define CMD_LCD_SETDDRAMADDRESS ((1 << DB7))
#define CMD_LCD_SETCGRAMADDRESS ((1 << DB6))
#define CMD_LCD_ENTRY_MODE_INC ((1 << DB2) | (1 << DB1))
#define CMD_LCD_ENTRY_MODE_DEC ((1 << DB2))
#define CMD_LCD_ENTRY_MODE_SHIFT_LEFT ((1 << DB2) | (1 << DB1) | (1 << DB0))
#define CMD_LCD_ENTRY_MODE_SHIFT_RIGHT ((1 << DB2) | (1 << DB0))
#define CMD_LCD_SHIFT_CURSOR_LEFT ((1 << DB4))
#define CMD_LCD_SHIFT_CURSOR_RIGHT ((1 << DB4) | (1 << DB2))
#define CMD_LCD_SHIFT_DISPLAY_LEFT ((1 << DB4) | (1 << DB3))
#define CMD_LCD_SHIFT_DISPLAY_RIGHT ((1 << DB4) | (1 << DB3) | (1 << DB2))

#define BG_BIT_POSITION 3  // background light
#define EN 2               // enable bit
#define RW 1               // read/write
#define RS 0               // register selectmake

// Display Functions
void lcd_I2C_init();
void lcd_command_4bit_mode(uint8_t cmd);
void lcd_write_char_4bit_mode(uint8_t c);
void lcd_set_cursor_by_address(uint8_t posAddress);
void lcd_set_cursor(uint8_t row, uint8_t column);
void createChar(uint8_t location, const uint8_t* charmap);

#endif
