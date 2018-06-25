#include "LightGroupScene.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
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
         std::shared_ptr<LightBulb const> pcl_light = LightBulb::get( str_light );
        if ( !pcl_light )
            return false;
        const LightBulbState& rcl_current_state = pcl_light->getTargetState();
        if ( rcl_current_state != rcl_state )
            return false;
    }
    return true;
}

void LightGroupScene::apply()
{
    GatewayAccess::instance().put("groups/"+m_strGroupId+"/scenes/"+id()+"/recall",{}, [this](const QJsonArray&){
        for ( const auto &[str_light, rcl_state] : m_mapLightStates )
        {
            LightBulb::get( str_light )->setTargetState( rcl_state );
        }
        emit sceneApplied();
    });
}

void LightGroupScene::save()
{
    for ( const auto &[str_light, rcl_state] : m_mapLightStates )
    {
        QJsonObject cl_object = rcl_state.toJson();
        GatewayAccess::instance().put("groups/"+m_strGroupId+"/scenes/"+id()+"/lights/"+str_light+"/state",QJsonDocument(cl_object).toJson(), [](const QJsonArray&){});
    }
}

void LightGroupScene::pickSettings(const QString &strLightId)
{
    std::shared_ptr<LightBulb const> pcl_light = LightBulb::get( strLightId );
    if ( !pcl_light )
        return;

    m_mapLightStates.insert_or_assign( strLightId, pcl_light->getCurrentState() );
    emit settingsRefreshed();
}

void LightGroupScene::setSceneData(const QJsonObject &rclObject)
{
    m_strName = rclObject.value("name").toString();
    m_mapLightStates.clear();
    for ( const QJsonValueRef& rcl_light : rclObject.value("lights").toArray() )
    {
        QJsonObject cl_light_settings = rcl_light.toObject();
        m_mapLightStates.insert_or_assign( cl_light_settings.value("id").toString(), LightBulbState::fromSceneSettings( cl_light_settings ) );
    }
    emit settingsRefreshed();

    if  ( isActive() )
        emit sceneApplied();
}

