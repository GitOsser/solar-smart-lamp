#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

class Delay
{
public:
    static volatile uint16_t counter;
    static void start(uint16_t s);
    static bool isDone();
    static void init();
};

#endif
