#include "CTLightBulb.h"
#include <QJsonObject>

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
    m_iMinTemperature = rclObject.value("ctmin").toInt();
    m_iMaxTemperature = rclObject.value("ctmax").toInt();
    LightBulb::setNodeData(rclObject);
}

void CTLightBulb::setTemperature(int iTemperature, float fTransitionTimeS)
{
    QJsonObject cl_object{ {"ct", iTemperature } };
    changeState(std::move(cl_object),fTransitionTimeS);
}

bool CTLightBulb::setStateData(const QJsonObject &rclObject)
{
    bool b_changed = LightBulb::setStateData(rclObject);
    int i_temperature = rclObject.value("ct").toInt();
    b_changed = b_changed || i_temperature != m_iTemperature;
    m_iTemperature = i_temperature;
    return b_changed;
}

