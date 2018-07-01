#include "DeviceNode.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "GatewayAccess.h"

bool DeviceNode::setReachable(bool bReachable)
{
    bool b_changed = m_bReachable != bReachable;
    m_bReachable = bReachable;
    return b_changed;
}

DeviceNode::~DeviceNode() = default;

void DeviceNode::setNodeData(const QJsonObject &rclObject)
{
    Node::setNodeData(rclObject);
    m_strManufacturer = rclObject.value("manufacturername").toString();
    m_strModelID      = rclObject.value("modelid").toString();
    m_strSWVersion    = rclObject.value("swversion").toString();
}

void DeviceNode::changeState( QJsonObject clObject, float fTransitionTimeS)
{
    if ( fTransitionTimeS >= 0 )
    {
        clObject.insert("transitiontime", static_cast<int>(fTransitionTimeS * 10) );
    }
    GatewayAccess::instance().put(nodeType()+"/"+id()+"/state", QJsonDocument(clObject).toJson(), [this](const QJsonArray&){ emit changeStateConfirmed(); });

}
