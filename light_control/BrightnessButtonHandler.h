#ifndef BRIGHTNESSBUTTONHANDLER_H
#define BRIGHTNESSBUTTONHANDLER_H

#include "RemoteControlButtonHandler.h"
#include <cstdint>

class BrightnessButtonHandler : public RemoteControlButtonHandler
{
public:
    BrightnessButtonHandler( int8_t iBrightnessStep );
    void workOn(LightGroup& rclGroup) override;
protected:
    int8_t m_iBrightnessStep;
};

#endif // BRIGHTNESSBUTTONHANDLER_H
