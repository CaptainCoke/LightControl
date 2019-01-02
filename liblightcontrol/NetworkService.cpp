#include "NetworkService.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebSocket>
#include "GatewayAccess.h"
#include "Nodes/LightBulb.h"
#include "Nodes/LightGroup.h"
#include "Nodes/Sensor.h"
#include "NodeTools.h"

NetworkService::NetworkService(QObject *parent)
: QObject(parent)
, m_pclWebSocket( std::make_unique<QWebSocket>() )
{
    connect( &GatewayAccess::instance(), &GatewayAccess::networkInfo, [this](const QString& strMessage){
        qDebug() << "Network info:" << strMessage;
    } );
    connect( &GatewayAccess::instance(), &GatewayAccess::networkError, [this](const QString& strMessage){
        qCritical() << "Network error:" << strMessage;
    } );
    connect( &GatewayAccess::instance(), &GatewayAccess::gatewayError, [this](const QString& strMessage){
        qCritical() << "Gateway error:" << strMessage;
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

    connect( m_pclWebSocket.get(), &QWebSocket::disconnected, [this]{
        if ( isUp() )
        {
            qInfo() << "websocket unexpectedly disconnected, starting again";
            refreshWebsocketPort();
        }
    } );
    connect( m_pclWebSocket.get(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [this](QAbstractSocket::SocketError){
        qCritical() << "Websocket error:" << m_pclWebSocket->errorString();
    } );
    connect( m_pclWebSocket.get(), &QWebSocket::textMessageReceived, this, &NetworkService::handlePushMessage );

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
    m_pclWebSocket->close();
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
        QString str_node_id = it_node.key();
        auto pcl_node = NodeFactory::get(str_node_id);
        bool b_new_node = pcl_node == nullptr;
        if ( pcl_node )
            pcl_node->setNodeData( cl_node );
        else
        {
            pcl_node = NodeFactory::create( str_node_id, cl_node );
            QObject::connect( pcl_node.get(), &Node::nodeDeleted, [str_node_id]{ NodeFactory::remove(str_node_id); } );
        }
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
    GatewayAccess::instance().get( LightBulb::node_type, [this](const QJsonObject& rclObject){
        if ( updateNodes<LightBulb>( rclObject ) )
            emit lightsChanged();
    });
}

void NetworkService::refreshGroupNodes()
{
    GatewayAccess::instance().get( LightGroup::node_type, [this](const QJsonObject& rclObject){
        updateNodes<LightGroup>( rclObject );
    });
}

void NetworkService::refreshWebsocketPort()
{
    GatewayAccess::instance().get( "config", [this](const QJsonObject& rclObject){
        updateWebsocketPort( rclObject );
    });
}

void NetworkService::handlePushMessage(const QString &strMessage)
{
    try
    {
        QJsonObject cl_message = QJsonDocument::fromJson( strMessage.toLocal8Bit() ).object();
        if ( cl_message.isEmpty() )
            throw std::runtime_error( "failed to interpret push message as JSON object" );

        // check: is the message an event?
        if ( cl_message.value("t").toString() != "event" )
            throw std::runtime_error( "unknown message type" );

        QString str_receiver = cl_message.value( "r" ).toString();
        if ( str_receiver.isNull() )
            throw std::runtime_error( "push event has no receiver" );

        auto it_receiver = m_mapPushMessageHandlers.find( str_receiver );
        if ( it_receiver != m_mapPushMessageHandlers.end() )
            it_receiver->second(cl_message);
        else
            qWarning() << "received push event for unknown receiver" << str_receiver;
    }
    catch( const std::exception& rclExc )
    {
        qCritical() << "failed to handle push message:" << rclExc.what() << "\n" << strMessage;
    }
}

void NetworkService::updateWebsocketPort( const QJsonObject& rclConfig )
{
    if ( int i_port = rclConfig.value("websocketport").toInt( 0 ) )
    {
        QUrl cl_websocket_url;
        cl_websocket_url.setHost(GatewayAccess::instance().getAddress().host());
        cl_websocket_url.setScheme("ws");
        cl_websocket_url.setPort( i_port );
        qDebug() << "opening websocket to" << cl_websocket_url;

        m_pclWebSocket->open( cl_websocket_url );
    }
    else
    {
        qWarning() << "no valid websocket port provided";
        return;
    }
}

template<class NodeFactory>
static void handleChangeEvent(const QJsonObject& rclMessage)
{
    if ( rclMessage.value("e") != "changed" )
        throw std::runtime_error( "event is not of type \"change\"" );
    QString str_node_id = rclMessage.value("id").toString();
    auto pcl_node = NodeFactory::get( str_node_id );
    if ( !pcl_node )
       throw std::runtime_error( "there is no "+NodeFactory::node_type.toStdString()+" node with id" + str_node_id.toStdString() );
    pcl_node->handlePushUpdate(rclMessage);
}

static void handleSceneCalled(const QJsonObject& rclMessage)
{
    if ( rclMessage.value("e") != "scene-called" )
        throw std::runtime_error( "event is not of type \"scene-called\"" );

    QString str_group_id = rclMessage.value("gid").toString();
    auto pcl_group = LightGroup::get( str_group_id );
    if ( !pcl_group )
        throw std::runtime_error( "there is no group with id " + str_group_id.toStdString() );
    pcl_group->handlePushUpdate(rclMessage);
}

void NetworkService::updateFullState(const QJsonObject& mapState)
{
    updateNodes<LightBulb>( mapState.value(LightBulb::node_type).toObject() );
    updateNodes<Sensor>( mapState.value(Sensor::node_type).toObject() );
    updateNodes<LightGroup>( mapState.value(LightGroup::node_type).toObject() );
    updateWebsocketPort( mapState.value("config").toObject() );

    m_mapPushMessageHandlers[Sensor::node_type]     = [](const QJsonObject& rclMessage){ handleChangeEvent<Sensor>(rclMessage); };
    m_mapPushMessageHandlers[LightBulb::node_type]  = [](const QJsonObject& rclMessage){ handleChangeEvent<LightBulb>(rclMessage); };
    m_mapPushMessageHandlers[LightGroup::node_type] = [](const QJsonObject& rclMessage){ handleChangeEvent<LightGroup>(rclMessage); };
    m_mapPushMessageHandlers["scenes"]              = [](const QJsonObject& rclMessage){ handleSceneCalled(rclMessage); };
}
