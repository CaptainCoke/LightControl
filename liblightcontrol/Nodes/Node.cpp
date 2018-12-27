#include "Node.h"
#include <QJsonObject>
#include "GatewayAccess.h"


void Node::initializeNode(const QString& strId)
{
    m_strId = strId;
    m_clStatePollingTimer.setInterval( 100 );
    connect( &m_clStatePollingTimer, &QTimer::timeout, this, &Node::refreshNode );
}

Node::~Node() = default;

void Node::setNodeData(const QJsonObject &rclObject)
{
    m_strName = rclObject.value("name").toString();
    m_strEtag = rclObject.value("etag").toString();
}

void Node::refreshNode()
{
    GatewayAccess::instance().get(nodeType()+"/"+id(), [this](const QJsonObject& rclObject){setNodeData(rclObject);}, m_strEtag);
}

void Node::deleteNode()
{
    GatewayAccess::instance().del(nodeType()+"/"+id(),[this]{ Q_EMIT nodeDeleted(uniqueId()); });
}

void Node::refreshPeriodically( uint32_t uiMilliseconds )
{
    m_clStatePollingTimer.start(uiMilliseconds);
}
