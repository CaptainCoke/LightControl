#include "RGBLightBulb.h"
#include <cmath>
#include <QJsonObject>
#include "LightBulbState.h"

void RGBLightBulb::setColor( LightColor clColor, float fTransitionTimeS)
{
    setTargetState( LightBulbState( getTargetState() ).setColor(std::move(clColor) ), fTransitionTimeS );
    changeToTargetStateIfNecessary();
}


int RGBLightBulb::hue() const
{
    if ( getCurrentState().hasColor() && getCurrentState().hasBrightness() )
        return getCurrentState().color().hue( getCurrentState().brightness() );
    else
        return 0;
}

uint8_t RGBLightBulb::saturation() const
{
    if ( getCurrentState().hasColor() && getCurrentState().hasBrightness() )
        return getCurrentState().color().saturation( getCurrentState().brightness() );
    else
        return 0;
}

bool RGBLightBulb::isRGBLight(const QJsonObject &rclObject)
{
    if ( !rclObject.value("hascolor").isBool() || !rclObject.value("hascolor").toBool() )
        return false;
    QJsonObject cl_state = rclObject.value("state").toObject();
    if ( cl_state.isEmpty() )
        return false;
    return LightBulbState::fromSceneSettings(cl_state).hasColor();
}

void RGBLightBulb::setHue(int iHue, float fTransitionTimeS)
{
    iHue = std::clamp( iHue, 0, 360 );
    setColor( LightColor::fromHSV(iHue, saturation(), brightness()), fTransitionTimeS );
}

void RGBLightBulb::setSaturation(uint8_t uiSaturation, float fTransitionTimeS)
{
    setColor( LightColor::fromHSV(hue(),uiSaturation,brightness()), fTransitionTimeS );
}

LightColor RGBLightBulb::color() const
{
    return getCurrentState().color();
}
