#include "CTLightBulb.h"
#include <QJsonObject>
#include "LightBulbState.h"

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
