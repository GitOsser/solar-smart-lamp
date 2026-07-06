#pragma once
#include <stdint.h>
#include <stdbool.h>

void uart_init();
void uart_send_char(char c);
void uart_send_string(const char* str);

// Buffer holding the most recently received line
extern volatile char uartBuffer[32];
extern volatile bool lineReady;
