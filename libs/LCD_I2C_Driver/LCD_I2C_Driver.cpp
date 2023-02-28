#include "LCD_I2C_Driver.h"

static void lcd_send_command(uint8_t data);


void lcd_I2C_init() {
    twi_init_master_transmitter();

    _delay_ms(5);
    lcd_send_command(0x30);
    _delay_ms(5);
    lcd_send_command(0x30);
    _delay_ms(1);
    lcd_send_command(0x30);
    _delay_ms(10);

    // set 4 bit mode
    lcd_send_command(0x20);  // 4bit mode //0x20 = 0010 0000 (DB5=1) = Function set (DL=0(4bit), N=0=1line, F=0 (5x8))
    _delay_ms(100);

    // set 4 bit mode 2 lines 5x8dots (figure 24 page 46)
    lcd_command_4bit_mode(CMD_LCD_SET_4BIT_2LINE);
    /*
    // lcd_send_command(0x20);  // 0x20 = 0010 0000  (DB5=1, DB4=0 (DL=0=4bit mode))
    // _delay_ms(100);
    // lcd_send_command(0x80);  // 0x80 = 1000 0000  (DB3=N=1(2lines), DB2=F=0 5x8dots) //2nd part of 4 bit
    // _delay_ms(100);
    */

    // Display off (DB3=1)
    lcd_command_4bit_mode(CMD_LCD_CONTROL_OFF);

    // Clear display
    lcd_command_4bit_mode(CMD_LCD_CLEAR);
    _delay_ms(5);

    // entry mode set
    lcd_command_4bit_mode(CMD_LCD_ENTRY_MODE_INC);

    // INITIALIZATION FINISHED (page 46, figure 24)

    // Display on
    lcd_command_4bit_mode(CMD_LCD_CONTROL_ON_NOBLINK);
}


static void lcd_send_command(uint8_t data) {
    // send data as a byte as is
    uint8_t data_EN = data | (1 << EN);
    uint8_t data_NOEN = data & ~(1 << EN);  // EN Must be off

    // Pulse EN bit
    twi_write8bit(LCD_ADDRESS, data_EN);
    _delay_us(50);
    twi_write8bit(LCD_ADDRESS, data_NOEN);
    _delay_us(50);
}


void lcd_command_4bit_mode(uint8_t cmd) {
    //     //As for the order of data transfer, the four high order bits (for 8-bit operation,
    //     DB4 to DB7) are transferred before the four low order bits (for 8-bit operation, DB0 to DB3).
    //     //    0x30 = 0011 0000
    //     // cmd bit = 7654 3210
    //     //     DBx = 7654 3210
    //     // 4 bit mode operation
    //     //   1. send DB(7-4) = cmd bit(7-4)
    //     //   2. send DB(3-0) = cmd bit(7-4)

    //     SDL Data Bits = 7 6 5 4  3  2  1  0
    //              (Datalines 7-4) BG EN RW RS
    uint8_t high_databits = cmd & 0xf0;
    uint8_t low_databits = (cmd << 4) & 0xf0;
    lcd_send_command(high_databits | (1 << BG_BIT_POSITION));
    lcd_send_command(low_databits | (1 << BG_BIT_POSITION));
}


void lcd_write_char_4bit_mode(uint8_t c) {
    uint8_t data_high = (c & 0xF0) | (1 << BG_BIT_POSITION | (1 << RS));  // RS=1
    uint8_t data_low = (c << 4 & 0xF0) | (1 << BG_BIT_POSITION | (1 << RS));

    lcd_send_command(data_high);
    lcd_send_command(data_low);
}


void lcd_set_cursor_by_address(uint8_t posAddress) {
    lcd_command_4bit_mode(CMD_LCD_SETDDRAMADDRESS | posAddress);
}


void lcd_set_cursor(uint8_t row, uint8_t column) {
    // First line:  0x00 -> 0x27
    // Second line: 0x40 -> 0x67 (40 chars, but display only has 16)
    uint8_t rowoffset = 0;  // default row = 0
    if (row > 0) {
        rowoffset = 0x40;  // 2nd row
    }
    lcd_command_4bit_mode(CMD_LCD_SETDDRAMADDRESS | (rowoffset + column));
}


void createChar(uint8_t location, const uint8_t* charmap) {
    location &= 0x7;  // we only have 8 locations 0-7
    lcd_command_4bit_mode(CMD_LCD_SETCGRAMADDRESS | (location << 3));
    for (int i = 0; i < 8; i++) {
        lcd_write_char_4bit_mode(charmap[i]);
    }
    _delay_us(100);
}
