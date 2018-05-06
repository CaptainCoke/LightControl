#include "LightGroupScene.h"
#include <QJsonObject>
#include <QJsonArray>
#include "LightBulbState.h"
#include "GatewayAccess.h"
#include "LightBulb.h"

LightGroupScene::LightGroupScene(const QString& strGroupId, const QString &strId, const QString &strName)
: m_strGroupId(strGroupId)
, m_strId(strId)
, m_strName(strName)
{
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

void LightGroupScene::apply()
{
    for ( const auto &[str_light, rcl_state] : m_mapLightStates )
    {
        LightBulb::get( str_light )->setToState( rcl_state );
    }
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
}

