#ifndef RGBLIGHTBULB_H
#define RGBLIGHTBULB_H

#include "DimmableLightBulb.h"
#include "LightColor.h"

class RGBLightBulb : public DimmableLightBulb
{
    Q_OBJECT
public:
    ~RGBLightBulb() override = default;

    LightColor color() const;

    static bool isRGBLight( const QJsonObject &rclObject );

    void setColor( LightColor clColor, float fTransitionTimeS = 0.f );

    int hue() const;
    uint8_t saturation() const;

public slots:
    void setHue( int iHue, float fTransitionTimeS = 0.f );
    void setSaturation( uint8_t uiSaturation, float fTransitionTimeS = 0.f );

protected:
    using DimmableLightBulb::DimmableLightBulb;
};

#endif // RGBLIGHTBULB_H
