#include "uart.h"

#include "avr/interrupt.h"
#define BAUD_PRESCALER (((F_CPU / (BAUD * 16UL))) - 1)
#define UBRRH (BAUD_PRESCALER >> 8)
#define UBRRL (BAUD_PRESCALER & 0xFF)

void UART::uart_9600(void) {
    UBRR0H = UBRRH;
    UBRR0L = UBRRL;
}

void UART::init() {
    UART::uart_9600();
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Asynchron 8N1 frame format
    UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);
    UCSR0B |= (1 << RXEN0) | (1 << RXCIE0);
}

void UART::transmitISR() {
    // to be called from TX complete ISR to transmit the next byte
    if (txReadPos != txWritePos) {
        UDR0 = serialTXBuffer[txReadPos];
        txReadPos++;
        if (txReadPos >= TX_BUFFER_SIZE) {
            txReadPos = 0;
        }
    }
}

void UART::receiveISR() {
    // to be called from RX complete ISR to read the next byte
    serialRXBuffer[rxWritePos] = UDR0;
    rxWritePos++;
    if (rxWritePos >= RX_BUFFER_SIZE) {
        rxWritePos = 0;
    }
}

serialBuffer_t UART::peekChar() {
    char ret = '\0';
    if (rxReadPos != rxWritePos) {
        ret = serialRXBuffer[rxReadPos];
    }
    return ret;
}

serialBuffer_t UART::getChar() {
    char ret = '\0';
    if (rxReadPos != rxWritePos) {
        ret = serialRXBuffer[rxReadPos];
        rxReadPos++;
        if (rxReadPos >= RX_BUFFER_SIZE) {
            rxReadPos = 0;
        }
    }
    return ret;
}

void UART::writeTransmitBuffer(const serialBuffer_t c[]) {
    uint8_t i = 0;
    while (c[i] != 0) {
        serialTXBuffer[txWritePos] = c[i];
        txWritePos++;
        if (txWritePos >= TX_BUFFER_SIZE) {
            txWritePos = 0;
        }
        ++i;
    }
    if (UCSR0A & (1 << UDRE0)) {  // transmit buffer empty
        UDR0 = 0;
        _delay_us(2000);
    }
}

uint8_t UART::receive_polling() {
    static uint8_t rxbyte;
    while (!(UCSR0A & (1 << RXC0))) {};
    rxbyte = UDR0;
    return rxbyte;
}

void UART::send_polling(const char* data) {
    uint8_t i = 0;
    while (data[i] != 0 && i != 255) {
        while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
        UDR0 = data[i];                    // Put data into buffer, sends the data
        i++;                                // increment counter
    }
    _delay_ms(5);
}
