#include "LightBulbState.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

LightBulbState::LightBulbState() = default;

LightBulbState::LightBulbState(const LightBulbState &) = default;
LightBulbState::LightBulbState(LightBulbState &&) = default;
LightBulbState& LightBulbState::operator=(const LightBulbState &) = default;
LightBulbState& LightBulbState::operator=(LightBulbState &&) = default;

LightBulbState::~LightBulbState() = default;


bool LightBulbState::hasOn() const
{
    return m_bOn.has_value();
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

bool LightBulbState::on() const
{
    return m_bOn.value();
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

LightBulbState& LightBulbState::setOn(bool bOn)
{
    m_bOn = bOn;
    return *this;
}

LightBulbState& LightBulbState::setBrightness( uint8_t uiBrightness )
{
    m_uiBrightness = uiBrightness;
    return *this;
}

LightBulbState& LightBulbState::setColor( LightColor clColor )
{
    m_clColor = clColor;
    m_clTemperature.reset();
    return *this;
}

LightBulbState& LightBulbState::setTemperature( LightTemperature clTemperature )
{
    m_clTemperature = clTemperature;
    m_clColor.reset();
    return *this;
}

bool LightBulbState::operator==(const LightBulbState &rclOther) const
{
    if ( hasOn() && ( !rclOther.hasOn() || on() != rclOther.on() ) ) return false;
    if ( hasOn() && !on() ) // special case: off is always the same as off, regardless the other settings...
        return true;
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

bool LightBulbState::operator!=(const LightBulbState &rclOther) const
{
    return !operator==(rclOther);
}

LightBulbState LightBulbState::operator-(const LightBulbState &rclOther) const
{
    LightBulbState cl_state;
    if ( hasOn() && (!rclOther.hasOn() || on() != rclOther.on()) )
        cl_state.setOn( on() );
    if ( hasBrightness() && ( !rclOther.hasBrightness() || brightness() != rclOther.brightness() ) )
        cl_state.setBrightness( brightness() );
    if ( hasTemperature() && ( !rclOther.hasTemperature() || rclOther.temperature() != temperature() ) )
        cl_state.setTemperature(temperature());
    if ( hasColor() && ( !rclOther.hasColor() || rclOther.color() != color() ) )
        cl_state.setColor(color());
    return cl_state;
}

void LightBulbState::updateSettingsFromJson(const QJsonObject &rclSettings)
{
    if ( rclSettings.value("on").isBool() )
        m_bOn = rclSettings.value("on").toBool();
    if ( rclSettings.value("bri").isDouble() )
        m_uiBrightness = rclSettings.value("bri").toInt();
    if ( rclSettings.value("ct").isDouble() )
        m_clTemperature = LightTemperature::fromMired( rclSettings.value("ct").toInt() );
    if ( rclSettings.value("x").isDouble() && hasColor() )
    {
        double x = rclSettings.value("x").toDouble();
        if ( x < 0 || x > 1 )
            qWarning() << "IGNORING silly x value " << x;
        else
            m_clColor =LightColor::fromXY( x, color().y() );
    }
    if ( rclSettings.value("y").isDouble() && hasColor() )
    {
        double y = rclSettings.value("y").toDouble();
        if ( y < 0 || y > 1 )
            qWarning() << "IGNORING silly y value " << y;
        m_clColor =LightColor::fromXY( color().x(), y );
    }
}

LightBulbState LightBulbState::fromSceneSettings(const QJsonObject &rclSettings)
{
    LightBulbState cl_state;

    QJsonValue cl_on = rclSettings.value("on");
    if ( !cl_on.isUndefined() )
        cl_state.setOn( rclSettings.value("on").toBool() );

    QJsonValue cl_bri = rclSettings.value("bri");
    if ( !cl_bri.isUndefined() )
        cl_state.setBrightness(static_cast<uint8_t>(cl_bri.toInt()));

    QJsonValue cl_color_mode = rclSettings.value("colormode");
    if ( !cl_color_mode.isUndefined() )
    {
        QString str_color_mode = cl_color_mode.toString();

        // with Ikea Lights, colormode does not seem to be a good indicator... rather look for "xy" or "hue" attributes
        if ( rclSettings.value("xy").isArray() && !rclSettings.value("xy").toArray().isEmpty() )
            str_color_mode = "xy";
        else if ( rclSettings.value("hue").isString() && rclSettings.value("sat").isString() )
            str_color_mode = "hs";

        if ( str_color_mode == "ct" )
            cl_state.setTemperature( LightTemperature::fromMired( rclSettings.value("ct").toInt() ) );
        else if ( str_color_mode == "xy" )
        {
            QJsonArray cl_xy = rclSettings.value("xy").toArray();
            if ( cl_xy.isEmpty() )
            {
                if ( rclSettings.value("x").isDouble() && rclSettings.value("y").isDouble() )
                    cl_state.setColor( LightColor::fromXY(
                        rclSettings.value("x").toDouble(),
                        rclSettings.value("y").toDouble() ) );
            }
            else if ( cl_xy[0].isDouble() && cl_xy[1].isDouble() )
                cl_state.setColor( LightColor::fromXY(
                    cl_xy[0].toDouble(),
                    cl_xy[1].toDouble() ) );
        }
        else if ( str_color_mode == "hs" && rclSettings.value("hue").isDouble() && rclSettings.value("sat").isDouble() )
            cl_state.setColor( LightColor::fromHSV(
                static_cast<int>((rclSettings.value("hue").toDouble() / 65535)*360),
                static_cast<uint8_t>(rclSettings.value("sat").toInt()),
                static_cast<uint8_t>(rclSettings.value("bri").toInt()) ));
    }

    return cl_state;
}

QJsonObject LightBulbState::toJson() const
{
    QJsonObject cl_object;
    if ( hasOn() )
        cl_object.insert( "on", on() );
    if ( hasBrightness() )
        cl_object.insert( "bri", static_cast<int>(brightness()) );
    if ( hasTemperature() )
    {
        cl_object.insert("colormode", "ct");
        cl_object.insert( "ct", static_cast<int>(temperature().mired()) );
    }
    if ( hasColor() )
    {
        cl_object.insert("colormode", "xy");
        //cl_object.insert( "xy", QJsonArray{color().x(),color().y()} );
        cl_object.insert( "x", color().x() );
        cl_object.insert( "y", color().y() );
    }
    return cl_object;
}

QStringList LightBulbState::getStateAsText() const
{
    QStringList lst_items;
    if ( hasOn() )
        lst_items << (on() ? "on" : "off");
    if ( hasBrightness() )
        lst_items << QString( "bri:%1" ).arg( brightness() );
    if ( hasTemperature() )
        lst_items << QString( "ct:%1" ).arg( temperature().mired() );
    if ( hasColor() )
        lst_items << QString("xy:%1,%2").arg( color().x() ).arg( color().y() );
    return lst_items;
}

QDebug &operator<<(QDebug &rclStream, const LightBulbState &rclState)
{
    QDebugStateSaver saver(rclStream);
    rclStream.nospace() << "{" << rclState.getStateAsText().join(";") << "}";
    return rclStream;
}
