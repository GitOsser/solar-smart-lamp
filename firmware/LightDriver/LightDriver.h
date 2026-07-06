#ifndef LIGHTDRIVER_H_
#define LIGHTDRIVER_H_

#include <stdint.h>

class LightDriver
{
public:
    LightDriver() = default;

    void initLightDriver(); 
    void activate();        
    void deactivate();      
    int sendLightSignal();  
};

#endif 
