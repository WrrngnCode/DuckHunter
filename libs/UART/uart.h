#ifndef UART_H_
#define UART_H_

#ifndef BAUD
#define BAUD 9600L
#endif

#include <avr/io.h>
#include <util/delay.h>

#ifndef TX_BUFFER_SIZE
#define TX_BUFFER_SIZE 60
#endif
#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE 60
#endif

typedef char serialBuffer_t;

class UART {
   public:
    void init();
    void writeTransmitBuffer(const serialBuffer_t c[]);
    void transmitISR();
    void send_polling(const char* data);
    void receiveISR();
    serialBuffer_t getChar(void);
    serialBuffer_t peekChar(void);
    uint8_t receive_polling();

   private:
    void uart_9600(void);
    volatile serialBuffer_t serialTXBuffer[TX_BUFFER_SIZE]{0};
    volatile serialBuffer_t serialRXBuffer[RX_BUFFER_SIZE]{0};
    volatile uint8_t txReadPos = 0;
    volatile uint8_t txWritePos = 0;
    volatile uint8_t rxReadPos = 0;
    volatile uint8_t rxWritePos = 0;
};

#endif