#include "System.h"
#include "LEDDriver/LEDDriver.h"
#include "LightDriver/LightDriver.h"
#include "MotionDriver/MotionDriver.h"
#include "Delay/Delay.h"
#include "Bluetooth/uart.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

SystemSettings EEMEM eeprom_settings;

void System::loadFromEEPROM()
{
    SystemSettings s;
    eeprom_read_block(&s, &eeprom_settings, sizeof(s));

    if (s.mode == 0xFF)
    {
        mode = AUTOMATIC;
        lightIntensity = 10;
        timer = 10;
        running = true;
        return;
    }

    mode = static_cast<Mode>(s.mode);
    lightIntensity = s.lightIntensity;
    timer = s.timer;
    running = s.running;
}

void System::saveToEEPROM()
{
    SystemSettings s;
    s.mode = mode;
    s.lightIntensity = lightIntensity;
    s.timer = timer;
    s.running = running;

    eeprom_update_block(&s, &eeprom_settings, sizeof(s));
}

System::System()
{
    loadFromEEPROM();
    timing = false;
}
void System::toggleMode(int value)
{
    this->mode = (value == 0) ? AUTOMATIC : MOTION;
}

void System::setTimer(int time)
{
    this->timer = time;
}

void System::startTimer()
{
    Delay::start(this->timer);
    timing = true;
}

void System::setIntensity(LEDDriver &led, int lightIntensity)
{
    this->lightIntensity = lightIntensity;
    led.setIntensity(lightIntensity);
}

void System::calculateSignal(volatile char *uartBuffer, LEDDriver &led)
{
    if (!uartBuffer || !uartBuffer[0])
        return;

    const volatile char *ptr = uartBuffer;
    bool changed = false;
    while (*ptr)
    {
        if (*ptr < 'A' || *ptr > 'Z')
        {
            ptr++;
            continue;
        }
        char id = *ptr++;
        int val = 0;
        while (*ptr >= '0' && *ptr <= '9')
        {
            val = val * 10 + (*ptr - '0');
            ptr++;
        }

        switch (id)
        {
        case 'A':
            toggleMode(val);

            changed = true;
            break;
        case 'B':
            lightIntensity = val;
            changed = true;
            break;
        case 'C':

            setTimer(val);
            changed = true;
            break;
        case 'D':
            running = (val == 1);
            if (!running && timing)
            {
                timing = false;
                led.setIntensity(0);
            }
            changed = true;
            break;
        case 'E':
            sendFeedback();
            break;
        default:
            break;
        }
    }
    if (changed)
        saveToEEPROM();
}

void System::sendFeedback()
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer),
             "A%dB%dC%dD%d\r\n",
             mode, lightIntensity, timer, running);
    uart_send_string(buffer);
}

void System::update(LightDriver &lightsensor, LEDDriver &led, MotionDriver &motion)
{

    if (running)
    {
        if (mode == AUTOMATIC)
        {
            lightsensor.activate();
            if (lightsensor.sendLightSignal())
            {
                led.setIntensity(this->lightIntensity);
            }
            else
            {
                led.setIntensity(0);
            }
        }
        else if (mode == MOTION)
        {
            lightsensor.deactivate();
            if (motion.detectMotion())
            {
                if (!timing)
                {
                    led.setIntensity(this->lightIntensity);
                    startTimer();
                }
            }
            if (timing && Delay::isDone())
            {
                led.setIntensity(0);
                timing = false;
            }
        }
    }
    else
    {
        led.setIntensity(0);
    }
}
