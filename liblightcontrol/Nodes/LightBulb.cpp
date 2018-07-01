#include "LightBulb.h"
#include <QJsonObject>
#include <QNetworkRequest>
#include <QtDebug>
#include "RGBLightBulb.h"
#include "CTLightBulb.h"
#include "LightBulbState.h"

const QString LightBulb::node_type{"lights"};

LightBulb::LightBulb(const QString& strId)
: DeviceNode(strId)
, m_pclCurrentState( std::make_unique<LightBulbState>(false) )
{
    connect( this, &DeviceNode::changeStateConfirmed, this, &Node::refreshNode );
    connect( this, &Node::stateChanged, this, &LightBulb::checkAndEnforceTargetState );
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

}

void LightBulb::checkAndEnforceTargetState()
{
    if ( getTargetState() != getCurrentState() )
    {
        float f_transition_time = 0.f;
        if ( m_clTargetStateTimepoint.isValid() )
            f_transition_time = std::max(0.0f,QDateTime::currentDateTime().msecsTo( m_clTargetStateTimepoint ) / 1000.0f);
        qDebug() << name() << "should be" << getTargetState() << "but is" << getCurrentState() << "\nenforcing target state in" << f_transition_time << "seconds";
        changeToState( getTargetState(), f_transition_time );
    }
    else
    {
         qDebug() << name() << "is now" << getCurrentState() << "(as it should be)";
    }
}

bool LightBulb::setStateData(const QJsonObject &rclObject)
{
    bool b_reachable = rclObject.value("reachable").toBool();
    LightBulbState cl_new_state = LightBulbState::fromSceneSettings( rclObject );

    bool b_changed = setReachable(b_reachable) || getCurrentState() != cl_new_state;
    getCurrentState() = std::move( cl_new_state );
    return b_changed;
}

void LightBulb::handlePushUpdate(const QJsonObject &rclObject)
{
    LightBulbState cl_new_state = getCurrentState();
    cl_new_state.updateSettingsFromJson( rclObject );
    bool b_changed = cl_new_state != getCurrentState();
    getCurrentState() = std::move( cl_new_state );
    if ( b_changed )
        emit stateChanged();
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

const QString& LightBulb::nodeType() const
{
    return node_type;
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

void LightBulb::setOn( bool bOn, float fTransitionTimeS )
{
    setTargetState( LightBulbState( getTargetState() ).setOn(bOn), fTransitionTimeS );
    checkAndEnforceTargetState();
}

void LightBulb::setBrightness(uint8_t uiBrightness, float fTransitionTimeS )
{
    setTargetState( LightBulbState( getTargetState() ).setBrightness(uiBrightness), fTransitionTimeS );
    checkAndEnforceTargetState();
}

void LightBulb::setTargetState(const LightBulbState &rclState, const QDateTime& rclWhen)
{
    qDebug() << name() << "should be" << rclState << "in" << QDateTime::currentDateTime().msecsTo( rclWhen ) << "milliseconds";
    if ( !m_pclTargetState )
        m_pclTargetState = std::make_unique<LightBulbState>( rclState );
    else
        *m_pclTargetState = rclState;
    m_clTargetStateTimepoint = rclWhen;
}

void LightBulb::setTargetState( const LightBulbState& rclState, float fSecondsInTheFuture )
{
    setTargetState( rclState, QDateTime::currentDateTime().addMSecs( fSecondsInTheFuture * 1000.f ) );
}
