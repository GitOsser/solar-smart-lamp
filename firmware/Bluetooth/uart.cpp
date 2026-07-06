#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile char uartBuffer[32];
volatile uint8_t uartIndex = 0;
volatile bool lineReady = false;

void uart_init()
{
    // USART1 on the Arduino Mega -> Bluetooth HC-05
    // Baud rate 9600 @ 16 MHz -> UBRR = 103
    uint16_t ubrr = 103;

    UBRR1H = (ubrr >> 8);
    UBRR1L = ubrr;

    UCSR1A = 0;
    UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1); // enable RX + TX + RX interrupt
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);               // 8-bit, 1 stopbit, no parity

    sei(); // Enable global interrupts
}

// --------------------------

void uart_send_char(char c)
{
    while (!(UCSR1A & (1 << UDRE1))) {}  // wait until the transmit buffer is ready
    UDR1 = c;
}

void uart_send_string(const char* str)
{
    while (*str)
    {
        uart_send_char(*str++);
    }
}

// --------------------------
// USART1 RX interrupt
// --------------------------
ISR(USART1_RX_vect)
{
    char c = UDR1;

    if (c == '\n' || c == '\r')
    {
        if (uartIndex > 0)
        {
            uartBuffer[uartIndex] = '\0';
            uartIndex = 0;
            lineReady = true; // signal to main() that a full line is ready
        }
        return;
    }

    if (uartIndex < sizeof(uartBuffer) - 1)
    {
        uartBuffer[uartIndex++] = c;
    }
}
