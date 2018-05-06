#include "CTLightBulb.h"
#include <QJsonObject>
#include "LightBulbState.h"

bool CTLightBulb::isCTLight(const QJsonObject &rclObject)
{
    if ( !rclObject.value("hascolor").isBool() || !rclObject.value("hascolor").toBool() )
        return false;
    QJsonObject cl_state = rclObject.value("state").toObject();
    if ( cl_state.isEmpty() || cl_state.find("ct") == cl_state.end() )
        return false;
    return true;
}

void CTLightBulb::setNodeData(const QJsonObject &rclObject)
{
    m_clMinTemperature = LightTemperature::fromMired( rclObject.value("ctmin").toInt() );
    m_clMaxTemperature = LightTemperature::fromMired( rclObject.value("ctmax").toInt() );
    LightBulb::setNodeData(rclObject);
}

void CTLightBulb::setToState(const LightBulbState &rclState)
{
    if ( rclState.hasTemperature() )
        setTemperature( rclState.temperature() );
    else if ( rclState.hasColor() )
        setTemperature( rclState.color().temperature() );
    LightBulb::setToState(rclState);
}

void CTLightBulb::setTemperature(LightTemperature clTemperature, float fTransitionTimeS)
{
    if ( clTemperature != m_clTemperature )
    {
        m_clTemperature = clTemperature;
        setMired( clTemperature.mired(), fTransitionTimeS );
    }
}

LightBulbState CTLightBulb::getCurrentState() const
{
    LightBulbState cl_state = LightBulb::getCurrentState();
    cl_state.setTemperature( m_clTemperature );
    return cl_state;
}

void CTLightBulb::setMired(uint16_t uiMired, float fTransitionTimeS)
{
    QJsonObject cl_object{ {"ct", uiMired } };
    changeState(std::move(cl_object),fTransitionTimeS);
}

bool CTLightBulb::setStateData(const QJsonObject &rclObject)
{
    bool b_changed = LightBulb::setStateData(rclObject);
    LightTemperature cl_temperature = LightTemperature::fromMired( rclObject.value("ct").toInt() );
    b_changed = b_changed || cl_temperature != m_clTemperature;
    m_clTemperature = cl_temperature;
    return b_changed;
}

