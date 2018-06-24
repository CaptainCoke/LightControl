#include "LightGroupScene.h"
#include <QJsonObject>
#include <QJsonArray>
#include "LightBulbState.h"
#include "GatewayAccess.h"
#include "LightBulb.h"

LightGroupScene::LightGroupScene(const QString& strGroupId, const QString &strId)
: m_strGroupId(strGroupId)
, m_strId(strId)
{
}

bool LightGroupScene::updateName(const QString &strName)
{
    bool b_changed = strName != m_strName;
    m_strName = strName;
    return b_changed;
}

const std::map<QString,LightBulbState> &LightGroupScene::getStates()
{
    return m_mapLightStates;
}

LightGroupScene::~LightGroupScene() = default;

void LightGroupScene::refreshSettings()
{
    GatewayAccess::instance().get("groups/"+m_strGroupId+"/scenes/"+id(), [this](const QJsonObject& rclObject){setSceneData(rclObject);});
}

bool LightGroupScene::isActive()
{
    for ( const auto &[str_light, rcl_state] : m_mapLightStates )
    {
        auto pcl_light = LightBulb::get( str_light );
        if ( !pcl_light )
            return false;
        LightBulbState cl_state = pcl_light->getCurrentState();
        if ( cl_state != rcl_state )
            return false;
    }
    return true;
}

void LightGroupScene::apply()
{
    for ( const auto &[str_light, rcl_state] : m_mapLightStates )
    {
        LightBulb::get( str_light )->setToState( rcl_state );
    }
    emit sceneApplied();
}

void LightGroupScene::setSceneData(const QJsonObject &rclObject)
{
    m_strName = rclObject.value("name").toString();
    m_mapLightStates.clear();
    for ( const QJsonValueRef& rcl_light : rclObject.value("lights").toArray() )
    {
        QJsonObject cl_light_settings = rcl_light.toObject();
        m_mapLightStates.emplace( cl_light_settings.value("id").toString(), LightBulbState::fromSceneSettings( cl_light_settings ) );
    }
    emit settingsRefreshed();

    if  ( isActive() )
        emit sceneApplied();
}

