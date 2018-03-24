#include "DeviceNode.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "GatewayAccess.h"

DeviceNode::DeviceNode(const QString& strId)
: m_strId(strId)
{
}

bool DeviceNode::setReachable(bool bReachable)
{
    bool b_changed = m_bReachable != bReachable;
    m_bReachable = bReachable;
    return b_changed;
}

DeviceNode::~DeviceNode() = default;

void DeviceNode::setNodeData(const QJsonObject &rclObject)
{
    m_strName         = rclObject.value("name").toString();
    m_strManufacturer = rclObject.value("manufacturername").toString();
    m_strModelID      = rclObject.value("modelid").toString();
    m_strSWVersion    = rclObject.value("swversion").toString();
    m_strEtag         = rclObject.value("etag").toString();
}

void DeviceNode::changeState( QJsonObject clObject, float fTransitionTimeS)
{
    if ( fTransitionTimeS >= 0 )
    {
        clObject.insert("transitiontime", static_cast<int>(fTransitionTimeS * 10) );
    }
    GatewayAccess::instance().put(nodeType()+"s/"+id()+"/state", QJsonDocument(clObject).toJson(), [](const QJsonArray&){});

}

void DeviceNode::refreshNode()
{
    GatewayAccess::instance().get(nodeType()+"s/"+id(), [this](const QJsonObject& rclObject){setNodeData(rclObject);}, m_strEtag);
}

void DeviceNode::deleteNode()
{
    GatewayAccess::instance().del(nodeType()+"s/"+id(),[this]{ Q_EMIT nodeDeleted(uniqueId()); });
}
