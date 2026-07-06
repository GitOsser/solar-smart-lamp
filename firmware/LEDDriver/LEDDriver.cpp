#define F_CPU 16000000UL

#include "LEDDriver.h"
#include <util/delay.h>


extern "C"
{
    uint8_t spi_10mhz_init(void);
    uint8_t spi_deinit(void);
    uint8_t spi_write_cmd(uint8_t *buf, uint16_t len);
    void ws_delay_ms(uint32_t ms);
    void ws_debug_print(const char *const fmt, ...);
}


LEDDriver::LEDDriver()
    : ledCount_(MAX_LEDS),
      currentIntensity_(0)
{
   
    DRIVER_WS2812B_LINK_INIT(&handle_, ws2812b_handle_t);

    
    DRIVER_WS2812B_LINK_SPI_10MHZ_INIT(&handle_, spi_10mhz_init);
    DRIVER_WS2812B_LINK_SPI_DEINIT(&handle_, spi_deinit);
    DRIVER_WS2812B_LINK_SPI_WRITE_COMMAND(&handle_, spi_write_cmd);
    DRIVER_WS2812B_LINK_DELAY_MS(&handle_, ws_delay_ms);
    DRIVER_WS2812B_LINK_DEBUG_PRINT(&handle_, ws_debug_print);
}


void LEDDriver::initLED()
{
    
    ws2812b_init(&handle_);

    
    currentIntensity_ = 0;
    for (uint32_t i = 0; i < ledCount_; ++i)
    {
        rgb_[i] = 0x000000;
    }

    ws2812b_write(&handle_, rgb_, ledCount_, temp_, TEMP_SIZE);
}

void LEDDriver::setIntensity(int intensity)
{
    
    if (intensity < 0)
        intensity = 0;
    if (intensity > 100)
        intensity = 100;

    this->currentIntensity_ = intensity;
    
    uint8_t level = static_cast<uint8_t>((currentIntensity_ * 255) / 100);

    
    uint32_t color = ((uint32_t)level << 16) |
                     ((uint32_t)level << 8) |
                     (uint32_t)level;

    for (uint32_t i = 0; i < ledCount_; ++i)
    {
        rgb_[i] = color;
    }

    
    ws2812b_write(&handle_, rgb_, ledCount_, temp_, TEMP_SIZE);
}

bool LEDDriver::isRunning() const
{
    return currentIntensity_ > 0;
}
