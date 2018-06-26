#include "NetworkService.h"
#include <QJsonObject>
#include "GatewayAccess.h"
#include "Nodes/LightBulb.h"
#include "Nodes/LightGroup.h"
#include "Nodes/Sensor.h"
#include "NodeTools.h"

NetworkService::NetworkService(QObject *parent)
: QObject(parent)
{
    connect( &GatewayAccess::instance(), &GatewayAccess::networkInfo, [this](const QString& strMessage){
        qDebug() << "Network info:" << strMessage;
    } );
    connect( &GatewayAccess::instance(), &GatewayAccess::networkError, [this](const QString& strMessage){
        qCritical() << "Network error:" << strMessage;
    } );

    connect( &GatewayAccess::instance(), &GatewayAccess::connectionRefused, [this]{
        static size_t ui_retry_count = 0;
        if ( isUp() )
        {
            ui_retry_count++;
            qInfo() << "restarting services ("<<ui_retry_count<<")";
            stop();
            QTimer::singleShot( 3000, this, &NetworkService::start );
        }
    } );

    m_clStatePollingTimer.setInterval( 3000 );
    connect( &m_clStatePollingTimer, &QTimer::timeout, this, &NetworkService::refreshLightNodes );
    connect( this, &NetworkService::lightsChanged, this, &NetworkService::refreshGroupNodes );
}

NetworkService::~NetworkService() = default;

bool NetworkService::isUp() const
{
    return m_bIsUp;
}

void NetworkService::start()
{
    m_bIsUp = true;
    GatewayAccess::instance().get( "", [this](const QJsonObject& rclObject){
        updateFullState(rclObject);
        m_clStatePollingTimer.start();
        emit networkUp();
    } );
}

void NetworkService::stop()
{
    m_bIsUp = false;
    m_clStatePollingTimer.stop();
    LightBulb::clearAllNodes();
    Sensor::clearAllNodes();
    LightGroup::clearAllNodes();
    emit networkDown();
}

template<class NodeFactory>
static bool updateNodes(const QJsonObject& mapNodeData)
{
    QStringList lst_new_ids, lst_all_ids;
    for ( auto it_node = mapNodeData.constBegin(); it_node != mapNodeData.constEnd(); ++it_node )
    {
        QJsonObject cl_node = it_node.value().toObject();
        auto pcl_node = NodeFactory::get(it_node.key());
        bool b_new_node = pcl_node == nullptr;
        if ( pcl_node )
            pcl_node->setNodeData( cl_node );
        else
            pcl_node = NodeFactory::create( it_node.key(), cl_node );
        if ( pcl_node->isEmpty() )
        {
            qDebug() << "not adding empty" << pcl_node->nodeType() << pcl_node->name();
            NodeFactory::remove( pcl_node->id() );
        }
        else
        {
            if ( b_new_node )
                lst_new_ids << pcl_node->id();
            lst_all_ids << pcl_node->id();
        }
    }

    if ( !lst_new_ids.isEmpty() )
    {
        qInfo() << "added" << lst_new_ids.size() << getTypeName<NodeFactory>();
        for ( const QString& str_id : lst_new_ids )
            qInfo() << "  *" << NodeFactory::get(str_id)->name();
    }

    // erase all nodes that have been removed...
    QStringList lst_removed_ids;
    for ( const auto & rcl_node : NodeFactory::getAll() )
        if ( !lst_all_ids.contains( rcl_node.first ) )
            lst_removed_ids << rcl_node.first;

    for ( const QString& str_id : lst_removed_ids )
        NodeFactory::remove(str_id);

    if ( !lst_removed_ids.isEmpty() )
    {
        qInfo() << "removed" << lst_removed_ids.size() << getTypeName<NodeFactory>();
        for ( const QString& str_id : lst_removed_ids )
            qInfo() << "  *" << NodeFactory::get(str_id)->name();
    }

    return !(lst_new_ids.isEmpty() && lst_removed_ids.isEmpty());
}

void NetworkService::refreshLightNodes()
{
    GatewayAccess::instance().get( "lights", [this](const QJsonObject& rclObject){
        if ( updateNodes<LightBulb>( rclObject ) )
            emit lightsChanged();
    });
}

void NetworkService::refreshGroupNodes()
{
    GatewayAccess::instance().get( "groups", [this](const QJsonObject& rclObject){
        updateNodes<LightGroup>( rclObject );
    });
}


void NetworkService::updateFullState(const QJsonObject& mapState)
{
    updateNodes<LightBulb>( mapState.value("lights").toObject() );
    updateNodes<Sensor>( mapState.value("sensors").toObject() );
    updateNodes<LightGroup>( mapState.value("groups").toObject() );
}
