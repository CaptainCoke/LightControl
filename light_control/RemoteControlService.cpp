#include "RemoteControlService.h"
#include <boost/core/demangle.hpp>
#include <QSettings>
#include <QJsonObject>
#include <QtDebug>
#include "Nodes/LightBulb.h"
#include "Nodes/LightGroup.h"
#include "Nodes/LightGroupScene.h"
#include "Nodes/Sensor.h"
#include "Nodes/RemoteControl.h"
#include "LightBulbState.h"
#include "GatewayAccess.h"

RemoteControlService::RemoteControlService(QObject *parent)
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
        if ( started() )
        {
            ui_retry_count++;
            qInfo() << "restarting service ("<<ui_retry_count<<")";
            stop();
            QTimer::singleShot( 3000, this, &RemoteControlService::start );
        }
    } );

    m_clStatePollingTimer.setInterval( 3000 );
    connect( &m_clStatePollingTimer, &QTimer::timeout, this, &RemoteControlService::refreshLightNodes );
    connect( this, &RemoteControlService::lightsChanged, this, &RemoteControlService::refreshGroupNodes );
}

bool RemoteControlService::started() const
{
    return m_bStarted;
}

RemoteControlService::~RemoteControlService() = default;

void RemoteControlService::start()
{
    m_bStarted = true;
    GatewayAccess::instance().get( "", [this](const QJsonObject& rclObject){
        updateFullState(rclObject);
        connectRemotesToGroups();
        m_clStatePollingTimer.start();
    } );
}

void RemoteControlService::stop()
{
    m_bStarted = false;
    m_clStatePollingTimer.stop();
    LightBulb::clearAllNodes();
    Sensor::clearAllNodes();
    LightGroup::clearAllNodes();
}

template<typename T>
static QString getTypeName()
{
    return QString::fromStdString(boost::core::demangle(typeid(T).name()));
}

template<class NodeFactory>
static bool updateNodes(const QJsonObject& mapNodeData)
{
    QStringList lst_new_ids, lst_all_ids;
    for ( auto it_node = mapNodeData.constBegin(); it_node != mapNodeData.constEnd(); ++it_node )
    {
        QJsonObject cl_node = it_node.value().toObject();
        auto pcl_node = NodeFactory::get(it_node.key());
        if ( pcl_node )
            pcl_node->setNodeData( cl_node );
        else
        {
            pcl_node = NodeFactory::create( it_node.key(), cl_node );
            lst_new_ids << pcl_node->id();
        }
        lst_all_ids << pcl_node->id();
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

void RemoteControlService::refreshLightNodes()
{
    GatewayAccess::instance().get( "lights", [this](const QJsonObject& rclObject){
        if ( updateNodes<LightBulb>( rclObject ) )
            emit lightsChanged();
    });
}

void RemoteControlService::refreshGroupNodes()
{
    GatewayAccess::instance().get( "groups", [this](const QJsonObject& rclObject){
        updateNodes<LightGroup>( rclObject );
    });
}


void RemoteControlService::updateFullState(const QJsonObject& mapState)
{
    updateNodes<LightBulb>( mapState.value("lights").toObject() );
    updateNodes<Sensor>( mapState.value("sensors").toObject() );
    updateNodes<LightGroup>( mapState.value("groups").toObject() );
}

template<typename T>
static std::shared_ptr<T> getByName( const QString& strName )
{
    std::list<std::shared_ptr<T>> lst_matches;
    for ( const auto& [str_id, pcl_node] : T::getAll() )
    {
        std::shared_ptr<T> pcl_element = std::dynamic_pointer_cast<T>(pcl_node);
        if ( pcl_element && pcl_element->name().compare( strName, Qt::CaseInsensitive ) == 0 )
        {
            qDebug() << "found" << getTypeName<T>() << str_id << "for name" << strName;
            lst_matches.emplace_back( std::move(pcl_element) );
        }
    }
    if ( lst_matches.empty() )
        throw std::runtime_error( qPrintable(QString( "%1 with name \"%2\" not found" ).arg(getTypeName<T>(),strName)) );
    else if ( lst_matches.size() > 1 )
        throw std::runtime_error( qPrintable(QString( "%1 name \"%2\" is ambiguous" ).arg(getTypeName<T>(),strName)) );
    else
        return lst_matches.front();
}

void switchLightsOnButtonPress(LightGroup& rclGroup, RemoteControl::Button button)
{
    switch ( button )
    {
    case RemoteControl::Button::Power:
    {
        bool b_all_lights_on = rclGroup.anyOn() ? false : true;
        qDebug() << "switching lights of group" << rclGroup.name() << (b_all_lights_on ? "on" : "off");
        for ( const auto &pcl_light : rclGroup.lights() )
            pcl_light->setOn(b_all_lights_on);
        break;
    }
    case RemoteControl::Button::Next:
    {
        if ( rclGroup.anyOn() )
        {
            rclGroup.setNextScene();
            qDebug() << "set group" << rclGroup.name() << "to scene" << rclGroup.getCurrentScene()->name();
        }
        break;
    }
    case RemoteControl::Button::Previous:
    {
        if ( rclGroup.anyOn() )
        {
            rclGroup.setPreviousScene();
            qDebug() << "set group" << rclGroup.name() << "to scene" << rclGroup.getCurrentScene()->name();
        }
        break;
    }
    case RemoteControl::Button::Brighter:
    {
        if ( rclGroup.anyOn() )
        {
            qDebug() << "increasing brightness of group" << rclGroup.name() << "by 32";
            for ( const auto &pcl_light : rclGroup.lights() )
                pcl_light->setBrightness(std::min(255,pcl_light->brightness()+32));
        }
        break;
    }
    case RemoteControl::Button::Dimmer:
    {
        if ( rclGroup.anyOn() )
        {
            qDebug() << "reducing brightness of group" << rclGroup.name() << "by 32";
            for ( const auto &pcl_light : rclGroup.lights() )
                pcl_light->setBrightness(std::max(1,pcl_light->brightness()-32));
        }
        break;
    }
    default:
        break;
    }
}

void RemoteControlService::connectRemotesToGroups()
{
    QSettings cl_settings;
    int num_connections = cl_settings.beginReadArray("remote_control");
    for (int i = 0; i < num_connections; ++i)
    {
        cl_settings.setArrayIndex(i);

        QString str_remote_name = cl_settings.value("remote").toString();
        QString str_group_name  = cl_settings.value("group").toString();

        qInfo() << "connecting remote control" <<  str_remote_name << "with group" << str_group_name;

        std::shared_ptr<RemoteControl> remote;
        std::shared_ptr<LightGroup>    group;
        try {
            remote = getByName<RemoteControl>(str_remote_name);
            group  = getByName<LightGroup>(str_group_name);
            QObject::connect( remote.get(), &RemoteControl::buttonPressed, [group](RemoteControl::Button b){ switchLightsOnButtonPress( *group, b ); } );
        } catch ( const std::exception& e ) {
            qCritical() << "finding remote or group for connection" << i << "failed: " << e.what();
        }
    }
    cl_settings.endArray();
}
