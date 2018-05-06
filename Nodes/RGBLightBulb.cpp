#include "RGBLightBulb.h"
#include <cmath>
#include <QJsonObject>
#include <QJsonArray>


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
    }
    else
    {
        LightColor cl_color = LightColor::fromXY( arr_xy[0].toDouble(), arr_xy[1].toDouble() );
        b_changed = b_changed || cl_color != m_clColor;
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

int RGBLightBulb::hue() const
{
    m_clColor.hue( brightness() );
}

uint8_t RGBLightBulb::saturation() const
{
    m_clColor.saturation( brightness() );
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
