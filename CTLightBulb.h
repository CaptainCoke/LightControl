#ifndef CTLIGHTBULB_H
#define CTLIGHTBULB_H

#include "LightBulb.h"

class CTLightBulb : public LightBulb
{
public:
    ~CTLightBulb() override = default;

    static bool isCTLight( const QJsonObject &rclObject );
protected:
    using LightBulb::LightBulb;
};

#endif // CTLIGHTBULB_H
