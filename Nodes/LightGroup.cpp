#include "LightGroup.h"
#include <QJsonObject>
#include <QJsonArray>
#include "LightBulb.h"
#include "LightGroupScene.h"

std::map<QString,std::shared_ptr<LightGroup>> LightGroup::s_mapGroups;

LightGroup::~LightGroup() = default;

std::list<std::shared_ptr<LightBulb>> LightGroup::lights() const
{
    std::list<std::shared_ptr<LightBulb>> lst_lights;
    for ( const QString& strId : m_lstLightIds )
        lst_lights.emplace_back( LightBulb::get(strId) );
    return lst_lights;
}

void LightGroup::setNodeData(const QJsonObject &rclObject)
{
    Node::setNodeData( rclObject );
    if ( setScenes( rclObject.value("scenes").toArray() ) | setLights( rclObject.value("lights").toArray() ) | setStateData( rclObject.value("state").toObject() ) )
         emit stateChanged();
    refreshPeriodically(1000);
}

std::shared_ptr<LightGroup> LightGroup::create(const QString &strId, const QJsonObject &rclObject)
{
    std::shared_ptr<LightGroup> pcl_group( new LightGroup(strId) );
    pcl_group->setNodeData( rclObject );
    s_mapGroups[strId] = pcl_group;
    return pcl_group;
}

std::shared_ptr<LightGroup> LightGroup::get(const QString &strId)
{
    auto it_group = s_mapGroups.find(strId);
    if ( it_group != s_mapGroups.end() )
        return it_group->second;
    else
        return nullptr;
}

void LightGroup::remove(const QString &strId)
{
    auto it_group = s_mapGroups.find(strId);
    if ( it_group != s_mapGroups.end() )
        s_mapGroups.erase( it_group );
}

QString LightGroup::nodeType() const
{
    return "group";
}

bool LightGroup::setStateData(const QJsonObject &rclObject)
{
    bool b_all_on = rclObject.value("all_on").toBool();
    bool b_any_on = rclObject.value("any_on").toBool();
    bool b_changed = b_all_on != m_bAllOn || b_any_on != m_bAnyOn;
    m_bAllOn = b_all_on;
    m_bAnyOn = b_any_on;
    return b_changed;
}

bool LightGroup::setLights(const QJsonArray &rclArray)
{
    QStringList lst_ids;
    for ( const QJsonValue& val : rclArray )
        lst_ids << val.toString();
    bool b_changed = lst_ids != m_lstLightIds;
    m_lstLightIds = std::move(lst_ids);
    return b_changed;
}

bool LightGroup::setScenes(const QJsonArray &rclArray)
{
    std::list<std::shared_ptr<LightGroupScene>> lst_scenes;
    for ( const QJsonValue& rcl_scene : rclArray )
    {
        QJsonObject cl_scene = rcl_scene.toObject();
        auto pcl_scene = std::make_shared<LightGroupScene>(id(), cl_scene.value("id").toString(), cl_scene.value("name").toString());
        connect( pcl_scene.get(), &LightGroupScene::settingsRefreshed, this, &LightGroup::stateChanged );
        pcl_scene->refreshSettings();
        lst_scenes.emplace_back( std::move(pcl_scene) );
    }
    bool b_changed = lst_scenes.size() != m_lstScenes.size();
    m_lstScenes = std::move(lst_scenes);
    return b_changed;
}
