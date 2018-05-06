#include "LightBulbState.h"
#include <QJsonObject>

LightBulbState::LightBulbState( bool bOn )
: m_bOn(bOn)
{
}

LightBulbState::~LightBulbState() = default;


bool LightBulbState::isOn() const
{
    return m_bOn;
}

bool LightBulbState::hasBrightness() const
{
    return m_uiBrightness.has_value();
}

uint8_t LightBulbState::brightness() const
{
    return m_uiBrightness.value();
}

void LightBulbState::setOn(bool bOn)
{
    m_bOn = bOn;
}

void LightBulbState::setBrightness( uint8_t uiBrightness )
{
    m_uiBrightness = uiBrightness;
}

void LightBulbState::setColor( LightColor clColor )
{
    m_clColor = clColor;
    m_iTemperature.reset();
}

void LightBulbState::setTemperature( int iTemperature )
{
    m_iTemperature = iTemperature;
    m_clColor.reset();
}

LightBulbState LightBulbState::fromSceneSettings(const QJsonObject &rclSettings)
{
    LightBulbState cl_state( rclSettings.value("on").toBool() );
    QJsonValue cl_bri = rclSettings.value("bri");
    if ( !cl_bri.isUndefined() )
        cl_state.setBrightness(static_cast<uint8_t>(cl_bri.toInt()));

    QJsonValue cl_color_mode = rclSettings.value("colormode");
    if ( !cl_color_mode.isUndefined() )
    {
        if ( cl_color_mode.toString() == "cv" )
            cl_state.setTemperature(rclSettings.value("ct").toInt());
        else if ( cl_color_mode.toString() == "xy" )
            cl_state.setColor( LightColor::fromXY(
                rclSettings.value("x").toDouble(),
                rclSettings.value("y").toDouble() ) );
    }

    return cl_state;
}
