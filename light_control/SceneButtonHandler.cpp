#include "SceneButtonHandler.h"
#include <QtDebug>
#include <Nodes/LightGroup.h>
#include <Nodes/LightGroupScene.h>

SceneButtonHandler::SceneButtonHandler(bool bForward)
: m_bForward(bForward)
{}

void SceneButtonHandler::workOn(LightGroup &rclGroup)
{
    if ( rclGroup.anyOn() )
    {
        if ( rclGroup.getCurrentScene() )
        {
            auto next_scene = m_bForward ? rclGroup.getNextScene() : rclGroup.getPreviousScene();

            if ( next_scene )
            {
                qDebug() << "set group" << rclGroup.name() << "to scene" << next_scene->name();
                next_scene->apply();
            }
            else
                qWarning() << "group" << rclGroup.name() << "has no" << ( m_bForward ? "next" : "previous" ) << "scene";
        }
        else
        {
            auto pcl_first_scene = rclGroup.scenes().begin()->second;
            qDebug() << "set group" << rclGroup.name() << "to first scene" << pcl_first_scene->name();
            pcl_first_scene->apply();
        }
    }
}

std::shared_ptr<SceneButtonHandler> SceneButtonHandler::nextScene()
{
    return std::make_shared<SceneButtonHandler>( true );
}

std::shared_ptr<SceneButtonHandler> SceneButtonHandler::previousScene()
{
    return std::make_shared<SceneButtonHandler>( false );
}
