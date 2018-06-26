#include "RemoteControlService.h"
#include <QSettings>
#include "GatewayAccess.h"
#include "Nodes/RemoteControl.h"
#include "Nodes/LightGroup.h"
#include "NodeTools.h"
#include "PowerButtonHandler.h"
#include "SceneButtonHandler.h"
#include "BrightnessButtonHandler.h"


RemoteControlService::RemoteControlService(QObject *parent)
: QObject(parent)
{
    m_mapButtonHandler.emplace( RemoteControl::Button::Power, std::make_shared<PowerButtonHandler>() );
    m_mapButtonHandler.emplace( RemoteControl::Button::Next, SceneButtonHandler::nextScene() );
    m_mapButtonHandler.emplace( RemoteControl::Button::Previous, SceneButtonHandler::previousScene() );
    m_mapButtonHandler.emplace( RemoteControl::Button::Brighter, std::make_shared<BrightnessButtonHandler>(32) );
    m_mapButtonHandler.emplace( RemoteControl::Button::Dimmer, std::make_shared<BrightnessButtonHandler>(-32) );
}

RemoteControlService::~RemoteControlService() = default;

void RemoteControlService::start()
{
    connectRemotesToGroups();
}

void RemoteControlService::stop()
{
}

void RemoteControlService::handleButtonPress( RemoteControl::Button button, LightGroup& rclGroup )
{
    auto it_handler = m_mapButtonHandler.find(button);
    if ( it_handler != m_mapButtonHandler.end() )
        it_handler->second->workOn(rclGroup);
    else
        qDebug() << "no handler for button " << static_cast<int>(button);
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
            QObject::connect( remote.get(), &RemoteControl::buttonPressed, [this,group](RemoteControl::Button b){ handleButtonPress( b, *group ); } );
        } catch ( const std::exception& e ) {
            qCritical() << "finding remote or group for connection" << i << "failed: " << e.what();
        }
    }
    cl_settings.endArray();
}
