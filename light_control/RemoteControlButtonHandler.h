#ifndef REMOTECONTROLBUTTONHANDLER_H
#define REMOTECONTROLBUTTONHANDLER_H

class LightGroup;

class RemoteControlButtonHandler
{
public:
    virtual void workOn( LightGroup& rclGroup ) = 0;
};

#endif // REMOTECONTROLBUTTONHANDLER_H
