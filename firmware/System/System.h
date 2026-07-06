#include "LightDriver/LightDriver.h"
#include "LEDDriver/LEDDriver.h"
#include "MotionDriver/MotionDriver.h"
#include <avr/eeprom.h>

enum Mode
{
	AUTOMATIC,
	MOTION
};

class System
{
public:
	System(); 

	
	void setTimer(int timer);

	void setIntensity(LEDDriver &led, int lightIntensity);

	
	void toggleMode(int mode);

	
	void calculateSignal();

	
	void update(LightDriver &lightsensor, LEDDriver &led, MotionDriver &motion);
	void calculateSignal(volatile char *uartBuffer, LEDDriver &led);
	
	void sendFeedback(); 

	void startTimer();

private:
	Mode mode;
	int lightIntensity;
	int timer;
	bool running;
	bool timing;

	
	void saveToEEPROM();
    void loadFromEEPROM();
};

struct SystemSettings {
    uint8_t mode;
    uint16_t lightIntensity;
    uint16_t timer;
    uint8_t running;
};

extern SystemSettings EEMEM eeprom_settings;