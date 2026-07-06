#include "MotionDriver.h"
#include <avr/io.h>

MotionDriver::MotionDriver(int pin)
    : sensorPin(pin)
{
    if (sensorPin == 22)
    {
     
        DDRA &= ~(1 << PA0);

     
    }
}

bool MotionDriver::detectMotion()
{
    if (sensorPin == 22)
    {
        
        return (PINA & (1 << PA0)) != 0;
    }

    return false;
}
