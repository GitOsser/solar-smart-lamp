#define F_CPU 16000000UL

#include <util/delay.h>
#include "LEDDriver/LEDDriver.h"
#include <avr/io.h>
#include "LightDriver/LightDriver.h"
#include "System/System.h"
#include "MotionDriver/MotionDriver.h"
#include "Delay/Delay.h"
#include "Bluetooth/uart.h"
#include <stdio.h>

#define LED_PIN PB7
#define LED_DDR DDRB
#define LED_PORT PORTB

int main(void)
{
    Delay::init();
    LEDDriver led;
    led.initLED();
    LightDriver lightsensor;
    lightsensor.initLightDriver();
    System system;
    MotionDriver motion(22);
    uart_init();

    while (1)
    {
        if (lineReady)
        {
            lineReady = false;
            system.calculateSignal(uartBuffer, led);
            system.sendFeedback();
        }
        system.update(lightsensor, led, motion);
    }
    return 0;
}
