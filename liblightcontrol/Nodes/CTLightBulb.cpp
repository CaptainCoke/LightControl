#include "CTLightBulb.h"
#include <QJsonObject>
#include "LightBulbState.h"

std::map<QString,uint8_t> CTLightBulb::s_mapModelTemperatureImprecisions = {{"TRADFRI bulb GU10 WS 400lm",5},{"TRADFRI bulb E27 WS opal 980lm",5}};

LightTemperature CTLightBulb::temperature() const
{
    if ( getCurrentState().hasTemperature() )
        return getCurrentState().temperature();
    else
        return m_clMinTemperature;
}

bool CTLightBulb::isCTLight(const QJsonObject &rclObject)
{
    if ( !rclObject.value("hascolor").isBool() || !rclObject.value("hascolor").toBool() )
        return false;
    QJsonObject cl_state = rclObject.value("state").toObject();
    if ( cl_state.isEmpty() )
        return false;
    return LightBulbState::fromSceneSettings(cl_state).hasTemperature();
}

void CTLightBulb::setNodeData(const QJsonObject &rclObject)
{
    m_clMinTemperature = LightTemperature::fromMired( rclObject.value("ctmin").toInt() );
    m_clMaxTemperature = LightTemperature::fromMired( rclObject.value("ctmax").toInt() );
    LightBulb::setNodeData(rclObject);
    if ( auto it_imprecision = s_mapModelTemperatureImprecisions.find( modelID() ); it_imprecision != s_mapModelTemperatureImprecisions.end() )
        m_uiTemperatureImprecisionMired = it_imprecision->second;
    else
        m_uiTemperatureImprecisionMired = 0;
}

void CTLightBulb::setTemperature(LightTemperature clTemperature, float fTransitionTimeS)
{
    setTargetState( LightBulbState( getTargetState() ).setTemperature( std::move(clTemperature) ), fTransitionTimeS );
    changeToTargetStateIfNecessary();
}

void CTLightBulb::setMired(uint16_t uiMired, float fTransitionTimeS)
{
    setTemperature( LightTemperature::fromMired( uiMired ), fTransitionTimeS );
}

bool CTLightBulb::isCloseEnoughToTargetState() const
{
    bool b_close_enough = LightBulb::isCloseEnoughToTargetState();
    if ( !b_close_enough && m_uiTemperatureImprecisionMired > 0 )
    {
        LightBulbState cl_diff  = getTargetState() - getCurrentState();

        if ( cl_diff.hasTemperature() )
        {
            bool b_brightness_ok = !cl_diff.hasBrightness() || cl_diff.brightness() == getCurrentState().brightness();
            bool b_on_ok = !cl_diff.hasOn() || cl_diff.on() == getCurrentState().on();

            // check if mired difference is below acceptable level
            int i_mired_diff = std::abs( getCurrentState().temperature().mired() - cl_diff.temperature().mired() );
            b_close_enough = b_brightness_ok && b_on_ok && i_mired_diff <= m_uiTemperatureImprecisionMired;
        }
    }
    return b_close_enough;
}
