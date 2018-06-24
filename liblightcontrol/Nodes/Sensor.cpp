#include "Sensor.h"
#include <QJsonObject>
#include "RemoteControl.h"

Sensor::~Sensor() = default;

void Sensor::setNodeData(const QJsonObject &rclObject)
{
    DeviceNode::setNodeData( rclObject );
    if ( setConfigData( rclObject.value("config").toObject() ) | setStateData(rclObject.value("state").toObject()) )
        emit stateChanged();
}

std::shared_ptr<Sensor> Sensor::createNode(const QString& strId, const QJsonObject &rclObject)
{
    std::shared_ptr<Sensor> pcl_sensor;
    if ( RemoteControl::isRemoteControl(rclObject) )
        pcl_sensor.reset( new RemoteControl(strId) );
    else
        pcl_sensor.reset( new Sensor(strId) );
    return pcl_sensor;
}

QString Sensor::nodeType() const
{
    return "sensor";
}

bool Sensor::setConfigData(const QJsonObject &rclObject)
{
    bool    b_on        = rclObject.value("on").toBool();
    bool    b_reachable = rclObject.value("reachable").toBool();
    uint8_t ui_battery  = static_cast<uint8_t>( rclObject.value("battery").toInt() );
    bool b_changed = setReachable(b_reachable) || b_on != m_bOn || ui_battery != m_uiBattery;
    m_bOn          = b_on;
    m_uiBattery    = ui_battery;
    return b_changed;
}

bool Sensor::setStateData(const QJsonObject &)
{
    return false;
}
