#include "LightGroupScene.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtDebug>
#include "LightBulbState.h"
#include "LightGroup.h"
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
    GatewayAccess::instance().get(LightGroup::node_type+"/"+m_strGroupId+"/scenes/"+id(), [this](const QJsonObject& rclObject){setSceneData(rclObject);});
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

void LightGroupScene::setTransitionTime(double fTransitionTimeS)
{
    m_fTransitionTimeS = fTransitionTimeS;
}

void LightGroupScene::notifyLightsOfActivation()
{
    QDateTime when = QDateTime::currentDateTime().addMSecs( m_fTransitionTimeS * 1000.f );
    for ( const auto &[str_light, rcl_state] : m_mapLightStates )
    {
        LightBulb::get( str_light )->setTargetState( rcl_state, when );
    }
}

void LightGroupScene::apply()
{
    notifyLightsOfActivation();
    GatewayAccess::instance().put(LightGroup::node_type+"/"+m_strGroupId+"/scenes/"+id()+"/recall",{}, [this](const QJsonArray&){
        emit sceneApplied();
    });
}

void LightGroupScene::save()
{
    for ( const auto &[str_light, rcl_state] : m_mapLightStates )
    {
        QJsonObject cl_object = rcl_state.toJson(true);
        cl_object.insert( "transitiontime", QJsonValue( static_cast<double>(m_fTransitionTimeS*10.f) ) );
        GatewayAccess::instance().put(LightGroup::node_type+"/"+m_strGroupId+"/scenes/"+id()+"/lights/"+str_light+"/state",QJsonDocument(cl_object).toJson(), [](const QJsonArray&){});
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
    m_fTransitionTimeS = -1;
    for ( const QJsonValueRef& rcl_light : rclObject.value("lights").toArray() )
    {
        QJsonObject cl_light_settings = rcl_light.toObject();
        QString str_light_id = cl_light_settings.value("id").toString();
        m_mapLightStates.insert_or_assign( str_light_id, LightBulbState::fromSceneSettings( cl_light_settings ) );
        float f_light_transition_time = static_cast<float>(cl_light_settings.value("transitiontime").toDouble()) / 10.0f;
        if ( m_fTransitionTimeS < 0 )
            m_fTransitionTimeS = f_light_transition_time;
        else if ( m_fTransitionTimeS != f_light_transition_time )
            qWarning() << name() << "transition time of light" << str_light_id << "differs from first found tranisition time";
    }
    m_fTransitionTimeS = std::max(0.f,m_fTransitionTimeS);
    emit settingsRefreshed();

    if  ( isActive() )
        emit sceneApplied();
}

