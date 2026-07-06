#include <avr/io.h>
#include <util/delay.h>
#include "driver_ws2812b.h"

uint8_t spi_10mhz_init(void)
{
    
    DDRB |= (1 << PB2) | (1 << PB1) | (1 << PB0);

    
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA);

    SPSR = (1 << SPI2X);

    return 0;
}

uint8_t spi_deinit(void)
{
    SPCR = 0;
    return 0;
}

uint8_t spi_write_cmd(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        SPDR = buf[i];
        while (!(SPSR & (1 << SPIF)))
        {
        }
    }
    return 0;
}

void ws_delay_ms(uint32_t ms)
{
    while (ms--)
        _delay_ms(1);
}

void ws_debug_print(const char *const fmt, ...)
{
    (void)fmt; 
}
