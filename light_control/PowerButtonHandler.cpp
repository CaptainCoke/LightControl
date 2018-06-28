#include "PowerButtonHandler.h"
#include <QtDebug>
#include "Nodes/LightGroup.h"
#include "Nodes/LightGroupScene.h"
#include "Nodes/LightBulb.h"

void PowerButtonHandler::workOn(LightGroup& rclGroup)
{
    bool b_turn_all_lights_on = rclGroup.anyOn() ? false : true;

    auto pcl_current_scene = rclGroup.getCurrentScene();
    if ( pcl_current_scene && b_turn_all_lights_on )
    {
        qDebug() << "applying last scene of group" << rclGroup.name() << ":" << pcl_current_scene->name();
        pcl_current_scene->apply();
    }
    else
    {
        qDebug() << "switching all lights of group" << rclGroup.name() << (b_turn_all_lights_on ? "on" : "off");
        for ( const auto &pcl_light : rclGroup.lights() )
            pcl_light->setOn(b_turn_all_lights_on);
    }
}
