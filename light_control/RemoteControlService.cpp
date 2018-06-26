#include "RemoteControlService.h"
#include <QSettings>
#include "Nodes/LightBulb.h"
#include "Nodes/LightGroupScene.h"
#include "Nodes/LightGroup.h"
#include "Nodes/RemoteControl.h"
#include "LightBulbState.h"
#include "GatewayAccess.h"
#include "NodeTools.h"

RemoteControlService::RemoteControlService(QObject *parent)
: QObject(parent)
{
}

RemoteControlService::~RemoteControlService() = default;

void RemoteControlService::start()
{
    connectRemotesToGroups();
}

void RemoteControlService::stop()
{
}

void switchLightsOnButtonPress(LightGroup& rclGroup, RemoteControl::Button button)
{
    switch ( button )
    {
    case RemoteControl::Button::Power:
    {
        bool b_all_lights_on = rclGroup.anyOn() ? false : true;

        auto pcl_current_scene = rclGroup.getCurrentScene();
        if ( pcl_current_scene && b_all_lights_on )
        {
            qDebug() << "applying last scene of group" << rclGroup.name() << ":" << pcl_current_scene->name();
            pcl_current_scene->apply();
        }
        else
        {
            qDebug() << "switching all lights of group" << rclGroup.name() << (b_all_lights_on ? "on" : "off");
            for ( const auto &pcl_light : rclGroup.lights() )
                pcl_light->setOn(b_all_lights_on);
        }
        break;
    }
    case RemoteControl::Button::Next:
    {
        if ( rclGroup.anyOn() )
        {
            if ( rclGroup.getCurrentScene() )
                qDebug() << "set group" << rclGroup.name() << "to next scene after scene" << rclGroup.getCurrentScene()->name();
            else
                qDebug() << "set group" << rclGroup.name() << "to first scene";
            rclGroup.setNextScene();
        }
        break;
    }
    case RemoteControl::Button::Previous:
    {
        if ( rclGroup.anyOn() )
        {
            if ( rclGroup.getCurrentScene() )
                qDebug() << "set group" << rclGroup.name() << "to scene before scene" << rclGroup.getCurrentScene()->name();
            else
                qDebug() << "set group" << rclGroup.name() << "to first scene";
            rclGroup.setPreviousScene();
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
