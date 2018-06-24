#include "LightBulbState.h"
#include <QJsonObject>
#include <QJsonArray>

LightBulbState::LightBulbState( bool bOn )
: m_bOn(bOn)
{
}

LightBulbState::LightBulbState(const LightBulbState &) = default;
LightBulbState::LightBulbState(LightBulbState &&) = default;
LightBulbState& LightBulbState::operator=(const LightBulbState &) = default;
LightBulbState& LightBulbState::operator=(LightBulbState &&) = default;

LightBulbState::~LightBulbState() = default;


bool LightBulbState::isOn() const
{
    return m_bOn;
}

bool LightBulbState::hasBrightness() const
{
    return m_uiBrightness.has_value();
}

bool LightBulbState::hasColor() const
{
    return m_clColor.has_value();
}

bool LightBulbState::hasTemperature() const
{
    return m_clTemperature.has_value();
}

uint8_t LightBulbState::brightness() const
{
    return m_uiBrightness.value();
}

LightColor LightBulbState::color() const
{
    return m_clColor.value();
}

LightTemperature LightBulbState::temperature() const
{
    return m_clTemperature.value();
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
    m_clTemperature.reset();
}

void LightBulbState::setTemperature( LightTemperature clTemperature )
{
    m_clTemperature = clTemperature;
    m_clColor.reset();
}

bool LightBulbState::operator==(const LightBulbState &rclOther)
{
    if ( isOn() != rclOther.isOn() ) return false;
    if ( hasBrightness()  && ( !rclOther.hasBrightness() || brightness() != rclOther.brightness() ) ) return false;
    if ( hasTemperature() )
    {
        if ( rclOther.hasTemperature() )
            return rclOther.temperature() == temperature();
        else if ( rclOther.hasColor() )
            return rclOther.color() == LightColor::fromTemperature(temperature());
        else
            return false;
    }
    if ( hasColor() )
    {
        if ( rclOther.hasTemperature() )
            return LightColor::fromTemperature(rclOther.temperature()) == color();
        else if ( rclOther.hasColor() )
            return rclOther.color() == color();
        else
            return false;
    }
    return true;
}

bool LightBulbState::operator!=(const LightBulbState &rclOther)
{
    return !operator==(rclOther);
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
        if ( cl_color_mode.toString() == "ct" )
            cl_state.setTemperature( LightTemperature::fromMired( rclSettings.value("ct").toInt() ) );
        else if ( cl_color_mode.toString() == "xy" )
            cl_state.setColor( LightColor::fromXY(
                rclSettings.value("x").toDouble(),
                rclSettings.value("y").toDouble() ) );
    }

    return cl_state;
}

QJsonObject LightBulbState::toJson() const
{
    QJsonObject cl_object;
    cl_object.insert( "on", isOn() );
    if ( hasBrightness() )
        cl_object.insert( "bri", brightness() );
    if ( hasTemperature() )
        cl_object.insert( "ct", temperature().mired() );
    if ( hasColor() )
        cl_object.insert( "xy", QJsonArray{color().x(),color().y()} );

    return cl_object;
}
