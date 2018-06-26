#include "SceneButtonHandler.h"
#include <QtDebug>
#include "Nodes/LightGroup.h"
#include "Nodes/LightGroupScene.h"

SceneButtonHandler::SceneButtonHandler(bool bForward)
: m_bForward(bForward)
{}

void SceneButtonHandler::workOn(LightGroup &rclGroup)
{
    if ( rclGroup.anyOn() )
    {
        if ( rclGroup.getCurrentScene() )
        {
            if ( m_bForward )
            {
                qDebug() << "set group" << rclGroup.name() << "to next scene after scene" << rclGroup.getCurrentScene()->name();
                rclGroup.setNextScene();
            }
            else
            {
                qDebug() << "set group" << rclGroup.name() << "to previous scene before scene" << rclGroup.getCurrentScene()->name();
                rclGroup.setPreviousScene();
            }
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
