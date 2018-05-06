#include "LightBulb.h"
#include <QJsonObject>
#include <QNetworkRequest>
#include "RGBLightBulb.h"
#include "CTLightBulb.h"
#include "LightBulbState.h"

std::map<QString,std::shared_ptr<LightBulb>> LightBulb::s_mapLights;

LightBulb::~LightBulb() = default;

void LightBulb::setNodeData(const QJsonObject &rclObject)
{
    DeviceNode::setNodeData( rclObject );
    if ( setStateData( rclObject.value("state").toObject() ) )
         emit stateChanged();
}

bool LightBulb::setStateData(const QJsonObject &rclObject)
{
    bool    b_reachable = rclObject.value("reachable").toBool();
    bool    b_on        = rclObject.value("on").toBool();
    uint8_t ui_bri      = static_cast<uint8_t>(rclObject.value("bri").toInt());
    bool b_changed = setReachable(b_reachable) || b_on != m_bOn || ui_bri != m_uiBrightness;
    m_bOn          = b_on;
    m_uiBrightness = ui_bri;
    return b_changed;
}

std::shared_ptr<LightBulb> LightBulb::create(const QString& strId, const QJsonObject &rclObject)
{
    std::shared_ptr<LightBulb> pcl_light;
    if ( RGBLightBulb::isRGBLight(rclObject) )
        pcl_light.reset( new RGBLightBulb(strId) );
    else if ( CTLightBulb::isCTLight(rclObject) )
        pcl_light.reset( new CTLightBulb(strId) );
    else
        pcl_light.reset( new LightBulb(strId) );
    pcl_light->setNodeData( rclObject );
    s_mapLights[strId] = pcl_light;
    return pcl_light;
}


std::shared_ptr<LightBulb> LightBulb::get(const QString& strId)
{
    auto it_light = s_mapLights.find(strId);
    if ( it_light != s_mapLights.end() )
        return it_light->second;
    else
        return nullptr;
}

void LightBulb::remove(const QString &strId)
{
    auto it_light = s_mapLights.find(strId);
    if ( it_light != s_mapLights.end() )
        s_mapLights.erase( it_light );
}

QString LightBulb::nodeType() const
{
    return "light";
}

void LightBulb::setToState(const LightBulbState &rclState)
{
    if ( rclState.hasBrightness() )
        setBrightness( rclState.brightness() );
    setOn( rclState.isOn() );
}

LightBulbState LightBulb::getCurrentState() const
{
    LightBulbState cl_state(m_bOn);
    cl_state.setBrightness(m_uiBrightness);
    return cl_state;
}

void LightBulb::setOn( bool bOn )
{
    if ( m_bOn != bOn )
    {
        changeState({ {"on", bOn } });
        m_bOn = bOn;
        emit stateChanged();
    }
}

void LightBulb::setBrightness(uint8_t uiBrightness, float fTransitionTimeS )
{
    if ( uiBrightness != m_uiBrightness )
    {
        changeState({ {"bri", uiBrightness } },fTransitionTimeS);
        m_uiBrightness = uiBrightness;
        emit stateChanged();
    }
}
