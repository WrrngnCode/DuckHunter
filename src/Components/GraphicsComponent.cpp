

#include "GraphicsComponent.h"

#include <stdio.h>

#include "Entity.h"
#include "Game.h"
#include "HC595.h"
#include "LCD_I2C_Driver.h"
#include "TM1637_Driver.h"
#include "customChars.h"


// Turn prescaler on/off to stop the timer
#define TOGGLE_T1_ON_OFF TCCR1B ^= ((1 << CS12) | (1 << CS10))
#define TOGGLE_T2_ON_OFF TCCR2B ^= ((1 << CS22) | (1 << CS21) | (1 << CS20))


inline void GraphicsComponent::Led16Driver::shiftLeft() {
    static uint8_t LSB = 0x01;
    pattern = pattern << 1 | LSB;
    if (pattern == 0xFFFF) LSB = 0;
    if (pattern == 0x0) LSB = 0x1;
}

inline void GraphicsComponent::Led16Driver::shiftRight() {
    static uint16_t MSB = 0x8000;
    pattern = pattern >> 1 | MSB;
    if (pattern == 0xFFFF) MSB = 0;
    if (pattern == 0x0) MSB = 0x8000;
}

void GraphicsComponent::Led16Driver::update() {
    shiftWrite(pattern);
}

void GraphicsComponent::shiftLedsLeft() {
    this->leds.shiftLeft();
}
void GraphicsComponent::shiftLedsRight() {
    this->leds.shiftRight();
}

void GraphicsComponent::init() {
    init_lcd_display();
    TM1637_init();
    shiftInit();
    welcome();
}

void GraphicsComponent::update(Entity** entities, uint8_t num_entities, uint8_t score, uint8_t alien_id) {
    static bool update_leds = false;

    // Clear old positions
    for (uint8_t i = 0; i < num_entities; ++i) {
        if (entities[i]->render_needed == true) {
            Position prev = entities[i]->getPrevPosition();
            lcd_set_cursor(prev.row, prev.column);
            lcd_write_char_4bit_mode(0x20);  // empty char
        }
    }

    // Draw new positions
    for (uint8_t i = 0; i < num_entities; ++i) {
        if (entities[i]->render_needed == true) {
            Position current = entities[i]->getPosition();

            lcd_set_cursor(current.row, current.column);
            lcd_write_char_4bit_mode(entities[i]->figure_id);
            entities[i]->render_needed = false;

            if (i == alien_id) {
                move_dir_t md = ((Alien*)(entities[i]))->getState();
                if (md == MOVING_LEFT) this->leds.shiftLeft();
                if (md == MOVING_RIGHT) this->leds.shiftRight();
            }
            update_leds = true;
        }
    }

    if (update_leds) {
        this->leds.update();

        TM1637_displayNumber(score, 10);
        if (score % 2) TOGGLE_T2_ON_OFF;
        TOGGLE_T1_ON_OFF;
        update_leds = false;
    }
}


void GraphicsComponent::welcome() {
    const uint8_t PLAY[] = {0b01110011, 0b00111000, 0b01110111, 0b01101110};
    TM1637_setSegmentBuffer(PLAY);
    TM1637_displaySegments(4, 0);

    shiftWrite(0x5555);
    _delay_ms(500);
    shiftWrite(0x5555 << 1);
    _delay_ms(500);
    shiftWrite(0);

    TM1637_displayNumber(8888, 10);
    _delay_ms(500);
    TM1637_clear();
    _delay_ms(500);
    TM1637_displayNumber(8888, 10);
    _delay_ms(500);
    TM1637_clear();
    TM1637_displayNumber(0, 10);
}

void GraphicsComponent::init_lcd_display() {
    twi_init_master_transmitter();
    _delay_ms(100);

    lcd_I2C_init();
    lcd_command_4bit_mode(CMD_LCD_CONTROL_ON_CURSOR_OFF);

    createChar(Game::ALIEN_ID, alien);
    createChar(Game::DUCK_ID, duck);

    // createChar(CLOCK2, clock2);
    // createChar(CLOCK3, clock3);
    // createChar(CLOCK4, clock4);
    // createChar(HOUSE1, housechar1);
    // createChar(HOUSE2, housechar3);
    // createChar(HOUSE4, housechar4);
    // lcd_write_char_4bit_mode('H');
    // lcd_write_char_4bit_mode('e');
    // lcd_write_char_4bit_mode('l');
    // lcd_write_char_4bit_mode('l');
    // lcd_write_char_4bit_mode('o');
    // lcd_write_char_4bit_mode('#');
    // lcd_set_cursor_by_address((1 << 6));
    // lcd_write_char_4bit_mode('a');
    // lcd_write_char_4bit_mode('b');
    // lcd_write_char_4bit_mode('c');
    // lcd_command_4bit_mode(CMD_LCD_SET_SECOND_LINE);
    // lcd_write_char_4bit_mode('d');
    // lcd_write_char_4bit_mode('e');
    // lcd_write_char_4bit_mode('f');
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_CURSOR_RIGHT);
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_CURSOR_RIGHT);
    // lcd_write_char_4bit_mode('1');
    // lcd_write_char_4bit_mode('2');
    // lcd_write_char_4bit_mode('3');
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_CURSOR_RIGHT);
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_DISPLAY_RIGHT);
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_DISPLAY_RIGHT);
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_DISPLAY_RIGHT);
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_DISPLAY_LEFT);
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_DISPLAY_LEFT);
    // lcd_write_char_4bit_mode('x');
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_CURSOR_LEFT);
    // lcd_command_4bit_mode(CMD_LCD_SHIFT_CURSOR_LEFT);
    // lcd_command_4bit_mode(CMD_LCD_ENTRY_MODE_SHIFT_LEFT);
    // lcd_write_char_4bit_mode('y');
    // lcd_write_char_4bit_mode('z');
    // lcd_set_cursor_by_address(0x40 + 8);
    // lcd_write_char_4bit_mode('q');
    // lcd_write_char_4bit_mode('w');
    // lcd_write_char_4bit_mode('s');
    // lcd_write_char_4bit_mode('<');
    // lcd_write_char_4bit_mode('R');
    // lcd_write_char_4bit_mode('R');
    // lcd_write_char_4bit_mode('R');
}
