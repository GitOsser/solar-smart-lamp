#define F_CPU 16000000UL

#include "LightDriver.h"
#include <avr/io.h>
#include <util/delay.h>


static void i2c_init(void)
{
    TWSR = 0x00;
    TWBR = 72; 
    TWCR = (1 << TWEN);
}

static void i2c_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
}

static void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

static void i2c_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
}

static uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}

static uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}



#define ADDR 0x29
#define CMD 0xA0

static void tsl_write(uint8_t reg, uint8_t val)
{
    i2c_start();
    i2c_write((ADDR << 1) | 0);
    i2c_write(CMD | reg);
    i2c_write(val);
    i2c_stop();
}

static uint8_t tsl_read(uint8_t reg)
{
    uint8_t val;
    i2c_start();
    i2c_write((ADDR << 1) | 0);
    i2c_write(CMD | reg);
    i2c_start();
    i2c_write((ADDR << 1) | 1);
    val = i2c_read_nack();
    i2c_stop();
    return val;
}



void LightDriver::initLightDriver()
{
    i2c_init();

    
    uint8_t id = tsl_read(0x12);

    
    tsl_write(0x01, 0x00);

    
    tsl_write(0x0F, 0x10);
}

void LightDriver::activate()
{
    
    tsl_write(0x00, 0x03);

    
    tsl_write(0x01, 0x20);

    
}

void LightDriver::deactivate()
{
    
    tsl_write(0x00, 0x00);
}

int LightDriver::sendLightSignal()
{
    
    uint16_t ch0 = ((uint16_t)tsl_read(0x15) << 8) | tsl_read(0x14);

    
    const int underThreshold = 3500;
    const int overThreshold = 4500;

    
    static bool isDark = false;

    
    if (!isDark && ch0 < underThreshold)
    {
        isDark = true;
    }
    else if (isDark && ch0 > overThreshold)
    {
        isDark = false;
    }

    
    return isDark ? 1 : 0;
}
