#include "Sensor.h"
#include <QJsonObject>
#include <QtDebug>
#include "RemoteControl.h"

const QString Sensor::node_type{"sensors"};

Sensor::~Sensor() = default;

void Sensor::setNodeData(const QJsonObject &rclObject)
{
    DeviceNode::setNodeData( rclObject );
    if ( setConfigData( rclObject.value("config").toObject() ) | setStateData(rclObject.value("state").toObject()) )
        emit stateChanged();
}

void Sensor::handlePushUpdate(const QJsonObject &rclObject)
{
    if ( QJsonObject cl_state = rclObject.value("state").toObject(); !cl_state.isEmpty() )
    {
        if ( setStateData( cl_state ) )
            emit stateChanged();
    }
    else if ( QJsonObject cl_config = rclObject.value("config").toObject(); !cl_config.isEmpty() )
    {
        if ( setConfigData( cl_config ) )
            emit stateChanged();
    }
    else
        qDebug() << name() << rclObject;
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

const QString& Sensor::nodeType() const
{
    return node_type;
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
