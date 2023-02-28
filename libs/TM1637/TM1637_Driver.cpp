#include "TM1637_Driver.h"
#include <util/delay.h>

#define SEGMENT_BUFFER_SIZE (4U)

static inline uint8_t tm1637_ACKcheck();
static inline void tm1637_endPacket();
static inline void tm1637_startPacket();
static inline void tm1637_readResult();
static void tm1637_writeByte(uint8_t b);

static uint8_t segmentbuffer[SEGMENT_BUFFER_SIZE] = {0x00, 0x00, 0x00, 0x00};

const uint8_t digitToSegment[] = {
    // XGFEDCBA
    0b00111111,  // 0
    0b00000110,  // 1
    0b01011011,  // 2
    0b01001111,  // 3
    0b01100110,  // 4
    0b01101101,  // 5
    0b01111101,  // 6
    0b00000111,  // 7
    0b01111111,  // 8
    0b01101111,  // 9
    0b01110111,  // A
    0b01111100,  // b
    0b00111001,  // C
    0b01011110,  // d
    0b01111001,  // E
    0b01110001   // F
};

// const uint8_t segmentMap[] = {
//     0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,  // 0-7
//     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71,  // 8-9, A-F
//     0x00};

// const uint8_t SEG_Done[] = {
//     SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
//     SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
//     SEG_C | SEG_E | SEG_G,                          // n
//     SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
// };

void TM1637_init() {
    cbi(DIO_DDR, DIO_PIN);  // DIO input
    cbi(CLK_DDR, CLK_PIN);  // Clock input

    cbi(CLK_PORT, CLK_PIN);  // Clock clear, Disable Internal Pull-Up
    cbi(DIO_PORT, DIO_PIN);  // DIO clear, Disable Internal Pull-Up

    _delay_ms(50);

    tm1637_startPacket();
    tm1637_writeByte(TM1637_CMD_SETUP);  // automatic address adding
    tm1637_readResult();
    tm1637_endPacket();

    _delay_us(100);
    TM1637_clear();
    _delay_us(100);
    TM1637_setBrightness(1);
}

// Lower SDO line while CLK line is high
static inline void tm1637_startPacket() {
    CLK_H;
    DIO_H;
    _delay_us(DELAY_US);
    DIO_L;
    _delay_us(DELAY_US);
}

void TM1637_clear(void) {
    tm1637_startPacket();
    tm1637_writeByte(TM1637_CMD_SET_DATA);
    // Write blank segment data bytes
    for (uint8_t i = 0; i < 4; i++) {
        tm1637_writeByte(0);
        tm1637_readResult();
    }

    tm1637_endPacket();
}

void TM1637_setSegmentBuffer(const uint8_t* buffer) {
    for (uint8_t i = 0; i < SEGMENT_BUFFER_SIZE; ++i) {
        segmentbuffer[i] = *(buffer + i);
    }
}

static void tm1637_writeByte(uint8_t b) {
    for (int i = 0; i < 8; ++i) {
        CLK_L;
        _delay_us(DELAY_US);
        if (b & 0x01) {
            DIO_H;
        } else {
            DIO_L;
        }
        _delay_us(DELAY_US);
        CLK_H;
        _delay_us(DELAY_US * 2);  // x2 to have a symmetrical clock cycle

        b >>= 1;
    }
}

void TM1637_setBrightness(uint8_t brightness) {
    // Brightness command:
    // 1000 0XXX = display off
    // 1000 1BBB = display on, brightness 0-7
    // X = don't care
    // B = brightness
    brightness &= (1 << 2) | (1 << 1) | (1 << 0);

    tm1637_startPacket();
    tm1637_writeByte(TM1637_CMD_DISPLAY_ON | brightness);
    tm1637_readResult();
    tm1637_endPacket();
}

static inline void tm1637_readResult() {
    // Send one CLK for acknowledgment
    CLK_L;
    _delay_us(DELAY_US);
    DIO_H;
    _delay_us(DELAY_US);
    CLK_H;
    _delay_us(DELAY_US);
    tm1637_ACKcheck();
}

static inline uint8_t tm1637_ACKcheck() {
    // Acknowledgment = SDO pulled low
    uint8_t ack = HIGH;

    if (bit_is_clear(DIO_PORT, DIO_PIN)) {
        ack = LOW;
    }
    _delay_us(DELAY_US);
    return ack;
}

// SDO line is pulled high while SCL line is high
static inline void tm1637_endPacket() {
    CLK_L;
    _delay_us(DELAY_US);
    DIO_L;
    _delay_us(DELAY_US);
    CLK_H;
    _delay_us(DELAY_US);
    DIO_H;
}

void TM1637_displaySegments(uint8_t length, uint8_t screenPos) {

    // Setup automatic address adding
    tm1637_startPacket();
    tm1637_writeByte(TM1637_CMD_SETUP);  // automatic address adding
    tm1637_readResult();
    tm1637_endPacket();

    // Send Segment data
    tm1637_startPacket();
    tm1637_writeByte(TM1637_CMD_SET_DATA | (screenPos & 0x3));
    tm1637_readResult();
    for (uint8_t i = 0; i < length; ++i) {
        tm1637_writeByte(segmentbuffer[i]);
        tm1637_readResult();
    }
    tm1637_endPacket();

    // Display on with brightness
    TM1637_setBrightness(2);
}

uint8_t TM1637_displayNumber(int32_t num, uint8_t base) {
    // Assume: 2 <= base <= 16
    uint8_t digits[4] = {0, 0, 0, 0};
    if (num == 0) {
        digits[3] = TM1637_ZERO;
        TM1637_setSegmentBuffer(digits);
        TM1637_displaySegments(4, 0);
        return 0;
    }
    const int16_t pos2 = base * base;
    const int16_t pos3 = pos2 * base;
    // Check if number fits in 4 digits, or 3 digits + minus sign
    if (num <= -pos3 ||
        (base < 16 && num >= pos3 * base) ||
        (base == 16 && num > 0xFFFF)) {
        return 1;
    }

    uint16_t abs_num = num < 0 ? -num : num;

    // Extract digits from num
    digits[0] = (abs_num / pos3) % base;
    digits[1] = (abs_num / pos2) % base;
    digits[2] = (abs_num / base) % base;
    digits[3] = abs_num % base;

    // Convert digits to segments
    uint8_t started = 0;
    for (uint8_t i = 0; i < SEGMENT_BUFFER_SIZE; i++) {
        if (started || digits[i]) {
            digits[i] = digitToSegment[digits[i]];
            if (!started && num < 0) {
                digits[i - 1] = TM1637_MINUS;
            }
            started = 1;
        } else {
            digits[i] = TM1637_BLANK;
        }
    }
    TM1637_setSegmentBuffer(digits);
    TM1637_displaySegments(4, 0);

    return 0;
}

void TM1637_displayDigits(const uint8_t* digits, uint8_t length, uint8_t screenPos) {
    for (uint8_t i = 0; i < length; i++) {
        if (i < SEGMENT_BUFFER_SIZE) {
            segmentbuffer[i] = digitToSegment[digits[i]];
        }
    }
    TM1637_displaySegments(length, screenPos);
}