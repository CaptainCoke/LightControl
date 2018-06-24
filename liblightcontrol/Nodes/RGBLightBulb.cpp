#include "RGBLightBulb.h"
#include <cmath>
#include <QJsonObject>
#include <QJsonArray>
#include "LightBulbState.h"

bool RGBLightBulb::setStateData(const QJsonObject &rclObject)
{
    bool b_changed = LightBulb::setStateData(rclObject);
    QJsonArray arr_xy = rclObject.value("xy").toArray();
    if ( arr_xy.isEmpty() )
    {
        double H = rclObject.value("hue").toDouble();
        double S = rclObject.value("sat").toDouble();
        double V = static_cast<double>(brightness())/255.0;
        LightColor cl_color = LightColor::fromHSV( H, S, V );
        b_changed = b_changed || cl_color != m_clColor;
        m_clColor = cl_color;
    }
    else
    {
        LightColor cl_color = LightColor::fromXY( arr_xy[0].toDouble(), arr_xy[1].toDouble() );
        b_changed = b_changed || cl_color != m_clColor;
        m_clColor = cl_color;
    }
    return b_changed;
}

void RGBLightBulb::setColor( LightColor clColor, float fTransitionTimeS)
{
    if ( clColor != m_clColor )
    {
        m_clColor = clColor;
        QJsonArray xy = {m_clColor.x(),m_clColor.y()};
        QJsonObject cl_object{ {"xy", xy } };
        changeState(std::move(cl_object),fTransitionTimeS);
        emit stateChanged();
    }
}

void RGBLightBulb::setToState(const LightBulbState &rclState)
{
    if ( rclState.hasColor() )
        setColor( rclState.color() );
    else if ( rclState.hasTemperature() )
        setColor( LightColor::fromTemperature( rclState.temperature() ) );
    LightBulb::setToState(rclState);
}

LightBulbState RGBLightBulb::getCurrentState() const
{
    LightBulbState cl_state = LightBulb::getCurrentState();
    cl_state.setColor( m_clColor );
    return cl_state;
}

int RGBLightBulb::hue() const
{
    return m_clColor.hue( brightness() );
}

uint8_t RGBLightBulb::saturation() const
{
    return m_clColor.saturation( brightness() );
}

bool RGBLightBulb::isRGBLight(const QJsonObject &rclObject)
{
    if ( !rclObject.value("hascolor").isBool() || !rclObject.value("hascolor").toBool() )
        return false;
    QJsonObject cl_state = rclObject.value("state").toObject();
    if ( cl_state.isEmpty() || cl_state.find("sat") == cl_state.end() )
        return false;
    return true;
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
