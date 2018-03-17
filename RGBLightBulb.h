#ifndef RGBLIGHTBULB_H
#define RGBLIGHTBULB_H

#include "LightBulb.h"

class RGBLightBulb : public LightBulb
{
    Q_OBJECT
public:
    ~RGBLightBulb() override = default;

    uint8_t saturation() const { return m_uiSaturation; }
    int hue() const { return m_iHue; }

    static bool isRGBLight( const QJsonObject &rclObject );

public slots:
    void setHue( int iHue, float fTransitionTimeS = 0.f );
    void setSaturation( uint8_t uiSaturation, float fTransitionTimeS = 0.f );

protected:
    using LightBulb::LightBulb;
    bool setStateData(const QJsonObject &rclObject) override;

    void setColor( float fTransitionTimeS = 0.f );
private:
    int m_iHue;
    uint8_t m_uiSaturation;
};

#endif // RGBLIGHTBULB_H
