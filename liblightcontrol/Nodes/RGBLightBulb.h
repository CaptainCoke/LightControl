#ifndef RGBLIGHTBULB_H
#define RGBLIGHTBULB_H

#include "LightBulb.h"
#include "LightColor.h"

class RGBLightBulb : public LightBulb
{
    Q_OBJECT
public:
    ~RGBLightBulb() override = default;

    LightColor color() const { return m_clColor; }

    static bool isRGBLight( const QJsonObject &rclObject );

    void setColor( LightColor clColor, float fTransitionTimeS = 0.f );
    void setToState( const LightBulbState& rclState ) override;
    LightBulbState getCurrentState() const override;

    int hue() const;
    uint8_t saturation() const;

public slots:
    void setHue( int iHue, float fTransitionTimeS = 0.f );
    void setSaturation( uint8_t uiSaturation, float fTransitionTimeS = 0.f );

protected:
    using LightBulb::LightBulb;
    bool setStateData(const QJsonObject &rclObject) override;

private:
    LightColor m_clColor = LightColor::fromXY(0.5,0.5);
};

#endif // RGBLIGHTBULB_H
