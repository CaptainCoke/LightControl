#include "LightBulb.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include "GatewayAccess.h"
#include "RGBLightBulb.h"
#include "CTLightBulb.h"

std::map<QString,std::shared_ptr<LightBulb>> LightBulb::s_mapLights;

LightBulb::LightBulb(const QString& strId)
: m_strId(strId)
{
}

LightBulb::~LightBulb() = default;

void LightBulb::setLightData(const QJsonObject &rclObject)
{
    m_strName         = rclObject.value("name").toString();
    m_strManufacturer = rclObject.value("manufacturername").toString();
    m_strModelID      = rclObject.value("modelid").toString();
    m_strSWVersion    = rclObject.value("swversion").toString();

    if ( setStateData( rclObject.value("state").toObject() ) )
         emit stateChanged();
}

bool LightBulb::setStateData(const QJsonObject &rclObject)
{
    bool b_changed   = false;
    bool b_reachable = rclObject.value("reachable").toBool();
    bool b_on        = rclObject.value("on").toBool();
    uint8_t ui_bri   = static_cast<uint8_t>(rclObject.value("bri").toInt());
    b_changed = b_reachable != m_bReachable || b_on != m_bOn || ui_bri != m_uiBrightness;
    m_bReachable   = b_reachable;
    m_bOn          = b_on;
    m_uiBrightness = ui_bri;
    return b_changed;
}

void LightBulb::changeState( QJsonObject clObject, float fTransitionTimeS)
{
    if ( fTransitionTimeS >= 0 )
    {
        clObject.insert("transitiontime", static_cast<int>(fTransitionTimeS * 10) );
    }
    GatewayAccess::instance().put("lights/"+m_strId+"/state", QJsonDocument(clObject).toJson(), [](const QJsonArray&){});

}

void LightBulb::refreshState()
{
    GatewayAccess::instance().get("lights/"+m_strId, [this](const QJsonObject& rclObject){setLightData(rclObject);});
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
    pcl_light->setLightData( rclObject );
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

void LightBulb::setOn( bool bOn )
{
    if ( m_bOn != bOn )
    {
        QJsonObject cl_object{ {"on", bOn } };
        GatewayAccess::instance().put("lights/"+m_strId+"/state", QJsonDocument(cl_object).toJson(), [](const QJsonArray&){});
        m_bOn = bOn;
        emit stateChanged();
    }
}

void LightBulb::setBrightness(uint8_t uiBrightness, float fTransitionTimeS )
{
    if ( uiBrightness != m_uiBrightness )
    {
        QJsonObject cl_object{ {"bri", uiBrightness } };
        changeState(std::move(cl_object),fTransitionTimeS);
        m_uiBrightness = uiBrightness;
        emit stateChanged();
    }
}
