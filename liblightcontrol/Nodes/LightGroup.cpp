#include "LightGroup.h"
#include <set>
#include <QJsonObject>
#include <QJsonArray>
#include "LightBulb.h"
#include "LightGroupScene.h"

LightGroup::~LightGroup() = default;

std::list<std::shared_ptr<LightBulb>> LightGroup::lights() const
{
    std::list<std::shared_ptr<LightBulb>> lst_lights;
    for ( const QString& strId : m_lstLightIds )
    {
        auto pcl_light = LightBulb::get(strId);
        if ( pcl_light )
            lst_lights.emplace_back( std::move(pcl_light) );
    }
    return lst_lights;
}

void LightGroup::setNodeData(const QJsonObject &rclObject)
{
    Node::setNodeData( rclObject );
    if ( setScenes( rclObject.value("scenes").toArray() ) | setLights( rclObject.value("lights").toArray() ) | setStateData( rclObject.value("state").toObject() ) )
         emit stateChanged();
    refreshPeriodically(5000);
}

bool LightGroup::anyOn() const
{
    for ( const auto& pcl_light : lights()  )
        if ( pcl_light->isOn() )
            return true;
    return false;
}

bool LightGroup::allOn() const
{
    for ( const auto& pcl_light : lights() )
        if ( !pcl_light->isOn() )
            return false;
    return true;
}

std::shared_ptr<LightGroup> LightGroup::createNode(const QString &strId, const QJsonObject &)
{
    return std::shared_ptr<LightGroup>( new LightGroup(strId) );
}

QString LightGroup::nodeType() const
{
    return "group";
}

std::shared_ptr<LightGroupScene> LightGroup::getCurrentScene() const
{
    auto it_scene = m_mapScenes.find( m_strCurrentScene );
    if ( it_scene != m_mapScenes.end() )
        return it_scene->second;
    else
        return nullptr;
}

void LightGroup::setNextScene()
{
    auto it_scene = m_mapScenes.find( m_strCurrentScene );
    it_scene++;
    if ( it_scene == m_mapScenes.end() && !m_mapScenes.empty() ) // wrap aound
        it_scene = m_mapScenes.begin();
    if ( it_scene != m_mapScenes.end() )
        it_scene->second->apply();
}

void LightGroup::setPreviousScene()
{
    auto it_scene = std::find_if( m_mapScenes.rbegin(), m_mapScenes.rend(), [this](const auto & rcl_entry){ return rcl_entry.first == m_strCurrentScene; } );
    it_scene++;
    if ( it_scene == m_mapScenes.rend() && !m_mapScenes.empty() ) // wrap aound
        it_scene = m_mapScenes.rbegin();
    if ( it_scene != m_mapScenes.rend() )
        it_scene->second->apply();
}

void LightGroup::setCurrentScene( const QString& strSceneId )
{
    if ( m_strCurrentScene != strSceneId )
    {
        m_strCurrentScene = strSceneId;
        emit stateChanged();
    }
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
    if ( b_changed )
    {
        for ( auto pcl_light : lights() )
            pcl_light->disconnect( this );
        m_lstLightIds = std::move(lst_ids);
        for ( auto pcl_light : lights() )
            connect( pcl_light.get(), &Node::stateChanged, this, &LightGroup::lightChanged );
    }
    return b_changed;
}

void LightGroup::lightChanged()
{
    for ( const auto & [str_id, pcl_scene] : scenes() )
    {
        if ( pcl_scene->isActive() )
        {
            setCurrentScene( str_id );
            return;
        }
    }
}

bool LightGroup::setScenes(const QJsonArray &rclArray)
{
    bool b_changed = false;
    std::set<QString> set_ids;
    for ( const QJsonValue& rcl_scene : rclArray )
    {
        QJsonObject cl_scene = rcl_scene.toObject();
        QString str_scene_id = cl_scene.value("id").toString();
        set_ids.insert(str_scene_id);
        auto it_scene = m_mapScenes.find( str_scene_id );
        std::shared_ptr<LightGroupScene> pcl_scene;
        if ( it_scene != m_mapScenes.end() ) {
            pcl_scene = it_scene->second;
        }
        else
        {
            pcl_scene = std::make_shared<LightGroupScene>(id(), str_scene_id);
            connect( pcl_scene.get(), &LightGroupScene::settingsRefreshed, this, &LightGroup::stateChanged );
            connect( pcl_scene.get(), &LightGroupScene::sceneApplied, this, [this]{
                auto pcl_scene = dynamic_cast<const LightGroupScene*>(sender());
                if (pcl_scene)
                    setCurrentScene(pcl_scene->id());
                });
            m_mapScenes.emplace( str_scene_id, pcl_scene );
            b_changed = true;
        }
        b_changed |= pcl_scene->updateName( cl_scene.value("name").toString() );
        pcl_scene->refreshSettings();
    }
    // remove any scenes that were not contained in array
    auto it_scene = m_mapScenes.begin();
    while ( it_scene != m_mapScenes.end() )
        if ( set_ids.find( it_scene->first ) == set_ids.end() )
        {
            it_scene = m_mapScenes.erase( it_scene );
            b_changed = true;
        }
        else
            it_scene++;
    auto pcl_current_scene = getCurrentScene();
    if ( !pcl_current_scene || !pcl_current_scene->isActive() )
    {
        m_strCurrentScene.clear();
        b_changed = true;
    }
    return b_changed;
}
