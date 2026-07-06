#pragma once

#include <stdint.h>

extern "C"
{
#include "driver_ws2812b.h"
}


class LEDDriver
{
public:
    LEDDriver();

    void initLED();

    void setIntensity(int intensity);

    bool isRunning() const;

private:
    ws2812b_handle_t handle_;

   
    static const uint32_t MAX_LEDS = 32;

  
    static const uint32_t TEMP_SIZE =
        MAX_LEDS * (WS2812B_EACH_RESET_BIT_FRAME_LEN / 8);

    uint32_t ledCount_;
    int currentIntensity_;

  
    uint32_t rgb_[MAX_LEDS];

  
    uint8_t temp_[TEMP_SIZE];
};
