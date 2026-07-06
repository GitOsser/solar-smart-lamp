#ifndef MOTIONDRIVER_H
#define MOTIONDRIVER_H

#include <avr/io.h>

class MotionDriver
{
private:
    int sensorPin;

public:
    MotionDriver(int pin);
    bool detectMotion();
};

#endif