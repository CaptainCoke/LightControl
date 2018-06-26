#ifndef POWERBUTTONHANDLER_H
#define POWERBUTTONHANDLER_H

#include "RemoteControlButtonHandler.h"

class PowerButtonHandler : public RemoteControlButtonHandler
{
public:
    void workOn(LightGroup& rclGroup ) override;
};

#endif // POWERBUTTONHANDLER_H
