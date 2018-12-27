#include "LightGroup.h"
#include <set>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include "LightBulb.h"
#include "LightGroupScene.h"
#include "LightBulbState.h"

const QString LightGroup::node_type{"groups"};

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
}

void LightGroup::handlePushUpdate(const QJsonObject &rclObject)
{
    if ( rclObject.value("e") == "changed" )
    {
        QJsonObject cl_state = rclObject.value("state").toObject();
        bool b_all_on = cl_state.value("all_on").toBool(m_bAllOn);
        bool b_any_on = cl_state.value("any_on").toBool(m_bAnyOn);
        bool b_changed = b_all_on != m_bAllOn || b_any_on != m_bAnyOn;
        m_bAllOn = b_all_on;
        m_bAnyOn = b_any_on;
        if ( b_changed )
            emit stateChanged();
    }
    else if ( rclObject.value("e") == "scene-called" )
    {
        QString str_scene_id = rclObject.value("scid").toString();
        setCurrentScene( str_scene_id );
        emit sceneCalled( str_scene_id );

        // enforce the called scene's light bulb settings
        auto pcl_scene = getCurrentScene();
        if ( pcl_scene )
        {
            qDebug() << name() << "scene"<<pcl_scene->name()<<"called. notifying lights";
            pcl_scene->notifyLightsOfActivation();
        }
    }
    else
        qDebug() << name() << rclObject;
}

bool LightGroup::anyOn() const
{
    for ( const auto& pcl_light : lights() )
        if ( pcl_light->isOn() )
            return true;
    return false;
}

bool LightGroup::allOn() const
{
    auto pcl_scene = getCurrentScene();
    for ( const auto& pcl_light : lights() )
    {
        bool b_should_be_on;
        if ( pcl_scene )
        {
            const auto & map_states = pcl_scene->getStates();
            auto it_light_state = map_states.find( pcl_light->id() );
            if ( it_light_state != map_states.end() && it_light_state->second.hasOn() && it_light_state->second.on() )
                b_should_be_on = true;
            else
                b_should_be_on = false;
        }
        else
            b_should_be_on = true;

        if ( !pcl_light->isOn() && b_should_be_on )
            return false;
    }
    return true;
}

std::shared_ptr<LightGroup> LightGroup::createNode(const QString &strId, const QJsonObject &)
{
    auto pcl_node = std::shared_ptr<LightGroup>( new LightGroup );
    pcl_node->initializeNode( strId );
    return pcl_node;
}

const QString& LightGroup::nodeType() const
{
    return node_type;
}

bool LightGroup::isEmpty() const
{
    return m_mapScenes.empty() && m_lstLightIds.empty();
}

std::shared_ptr<LightGroupScene> LightGroup::getCurrentScene() const
{
    auto it_scene = m_mapScenes.find( m_strCurrentScene );
    if ( it_scene != m_mapScenes.end() )
        return it_scene->second;
    else
        return nullptr;
}

std::shared_ptr<LightGroupScene> LightGroup::getNextScene() const
{
    auto it_scene = m_mapScenes.find( m_strCurrentScene );
    it_scene++;
    if ( it_scene == m_mapScenes.end() && !m_mapScenes.empty() ) // wrap aound
        it_scene = m_mapScenes.begin();
    if ( it_scene != m_mapScenes.end() )
        return it_scene->second;
    else
        return nullptr;
}

std::shared_ptr<LightGroupScene> LightGroup::getPreviousScene() const
{
    auto it_scene = std::find_if( m_mapScenes.rbegin(), m_mapScenes.rend(), [this](const auto & rcl_entry){ return rcl_entry.first == m_strCurrentScene; } );
    it_scene++;
    if ( it_scene == m_mapScenes.rend() && !m_mapScenes.empty() ) // wrap aound
        it_scene = m_mapScenes.rbegin();
    if ( it_scene != m_mapScenes.rend() )
        return it_scene->second;
    else
        return nullptr;
}

void LightGroup::setNextScene()
{
    if ( auto next_scene = getNextScene() )
        next_scene->apply();
}

void LightGroup::setPreviousScene()
{
    if ( auto previous_scene = getPreviousScene() )
        previous_scene->apply();
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
    if ( !pcl_current_scene || ( anyOn() && !pcl_current_scene->isActive() ) )
    {
        m_strCurrentScene.clear();
        b_changed = true;
    }
    return b_changed;
}
