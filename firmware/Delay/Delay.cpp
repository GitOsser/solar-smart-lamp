#include "Delay.h"

volatile uint16_t Delay::counter = 0;

ISR(TIMER1_COMPA_vect)
{
    if (Delay::counter > 0)
        Delay::counter--;
}

void Delay::init()
{
    cli();

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    TCCR1B |= (1 << WGM12);  
    OCR1A = 2499;            
    TIMSK1 |= (1 << OCIE1A); 

    TCCR1B |= (1 << CS11) | (1 << CS10); 

    sei();
}

void Delay::start(uint16_t s)
{
    counter = s * 100;
}

bool Delay::isDone()
{
    return counter == 0;
}
