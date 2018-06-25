#include "LightBulb.h"
#include <QJsonObject>
#include <QNetworkRequest>
#include "RGBLightBulb.h"
#include "CTLightBulb.h"
#include "LightBulbState.h"


LightBulb::LightBulb(const QString& strId)
: DeviceNode(strId)
, m_pclCurrentState( std::make_unique<LightBulbState>(false) )
{
    connect( this, &DeviceNode::changeStateConfirmed, this, &Node::refreshNode );
}

LightBulb::~LightBulb() = default;

bool LightBulb::isOn() const
{
    return getCurrentState().isOn();
}

uint8_t LightBulb::brightness() const
{
    if ( getCurrentState().hasBrightness() )
        return getCurrentState().brightness();
    else
        return 0;
}

void LightBulb::setNodeData(const QJsonObject &rclObject)
{
    DeviceNode::setNodeData( rclObject );
    if ( setStateData( rclObject.value("state").toObject() ) )
         emit stateChanged();
    if ( getTargetState() != getCurrentState() )
        changeToState( getTargetState() );
    refreshPeriodically(1000);
}

bool LightBulb::setStateData(const QJsonObject &rclObject)
{
    bool b_reachable = rclObject.value("reachable").toBool();
    LightBulbState cl_new_state = LightBulbState::fromSceneSettings( rclObject );

    bool b_changed = setReachable(b_reachable) || getCurrentState() != cl_new_state;
    getCurrentState() = std::move( cl_new_state );
    return b_changed;
}

std::shared_ptr<LightBulb> LightBulb::createNode(const QString& strId, const QJsonObject &rclObject)
{
    std::shared_ptr<LightBulb> pcl_light;
    if ( RGBLightBulb::isRGBLight(rclObject) )
        pcl_light.reset( new RGBLightBulb(strId) );
    else if ( CTLightBulb::isCTLight(rclObject) )
        pcl_light.reset( new CTLightBulb(strId) );
    else
        pcl_light.reset( new LightBulb(strId) );
    return pcl_light;
}

QString LightBulb::nodeType() const
{
    return "light";
}

void LightBulb::changeToState(const LightBulbState &rclState, float fTransitionTimeSeconds )
{
    changeState( rclState.toJson(), fTransitionTimeSeconds );
}

const LightBulbState& LightBulb::getCurrentState() const
{
    return *m_pclCurrentState;
}

LightBulbState& LightBulb::getCurrentState()
{
    return *m_pclCurrentState;
}

const LightBulbState& LightBulb::getTargetState() const
{
    if ( m_pclTargetState )
        return *m_pclTargetState;
    else
        return *m_pclCurrentState;
}

LightBulbState& LightBulb::getTargetState()
{
    if ( !m_pclTargetState )
        m_pclTargetState = std::make_unique<LightBulbState>( getCurrentState() );
    return *m_pclTargetState;
}

void LightBulb::setOn( bool bOn )
{
    getTargetState().setOn(bOn);
    if ( getTargetState() != getCurrentState() )
        changeToState( getTargetState() );
}

void LightBulb::setBrightness(uint8_t uiBrightness, float fTransitionTimeS )
{
    getTargetState().setBrightness(uiBrightness);
    if ( getTargetState() != getCurrentState() )
        changeToState( getTargetState(), fTransitionTimeS );
}

void LightBulb::setTargetState(const LightBulbState &rclState)
{
    getTargetState() = rclState;
}
