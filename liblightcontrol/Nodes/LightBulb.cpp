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
, m_pclCurrentState( std::make_unique<LightBulbState>() )
{
    m_clStateLostGracePeriod.setInterval(250);
    m_clStateLostGracePeriod.setSingleShot(true);
    connect( this, &DeviceNode::changeStateConfirmed, this, &Node::refreshNode );
    connect( this, &LightBulb::targetStateLost, this, &LightBulb::onFalseState );
    connect( &m_clStateLostGracePeriod, &QTimer::timeout, this, &LightBulb::onFalseState );

}

LightBulb::~LightBulb() = default;

bool LightBulb::isOn() const
{
    if ( getCurrentState().hasOn() )
        return getCurrentState().on();
    else
        return false;
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

    if ( !m_bIsInTargetState )
    {
        qDebug() << name() << "periodic check found light in"<<getCurrentState();
        reactOnTargetState();
    }
}

void LightBulb::changeToTargetStateIfNecessary()
{
    if ( !m_bIsInTargetState )
    {
        float f_transition_time = 0.f;
        if ( m_clTargetStateTimepoint.isValid() )
            f_transition_time = std::max(0.0f,QDateTime::currentDateTime().msecsTo( m_clTargetStateTimepoint ) / 1000.0f);
        LightBulbState cl_diff = getTargetState() - getCurrentState();
        qDebug() << "currently in" << getCurrentState() << " --> sending diff"<< cl_diff <<"tba in" << f_transition_time << "seconds";
        putStateOnLightBulb( cl_diff, f_transition_time );
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
    cl_new_state.updateSettingsFromJson( rclObject.value("state").toObject() );

    bool b_changed = cl_new_state != getCurrentState();

    LightBulbState cl_change = cl_new_state - getCurrentState();
    if ( b_changed )
    {
        qDebug() << name() << "External push" << cl_change << " to " << cl_new_state << "(was" << getCurrentState() << ")";
        getCurrentState() = cl_new_state;
        emit stateChanged();
    }
    else
        qDebug() << name() << "External push" << cl_change << ", but state remains same ("<< getCurrentState() <<")";

    reactOnTargetState();
}

void LightBulb::reactOnTargetState()
{
    bool b_is_in_target_state = getTargetState() == getCurrentState();
    LightBulbState cl_diff  = getTargetState() -  getCurrentState();
    if ( b_is_in_target_state && m_bIsInTargetState )
        qDebug() << "  --> still in target state.";
    else if ( b_is_in_target_state && !m_bIsInTargetState )
    {
        qDebug() << "  --> target state reached.";
        m_bIsInTargetState = true;
        emit targetStateReached();
    }
    else if ( !b_is_in_target_state && !m_bIsInTargetState )
    {
        qDebug() << "  --> waiting for target state. Diff:" << cl_diff;
        onFalseState();
    }
    else
    {
        qDebug() << "  --> target state lost. Diff:" << cl_diff;
        m_bIsInTargetState = false;
        emit targetStateLost();
    }
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

void LightBulb::putStateOnLightBulb(const LightBulbState &rclState, float fTransitionTimeSeconds )
{
    emit targetStateChanged();
    changeState( rclState.toJson(false), fTransitionTimeSeconds );
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

bool LightBulb::isInTargetState() const
{
    return m_bIsInTargetState;
}

void LightBulb::setOn( bool bOn, float fTransitionTimeS )
{
    setTargetState( LightBulbState( getTargetState() ).setOn(bOn), fTransitionTimeS );
    changeToTargetStateIfNecessary();
}

void LightBulb::setBrightness(uint8_t uiBrightness, float fTransitionTimeS )
{
    setTargetState( LightBulbState( getTargetState() ).setBrightness(uiBrightness), fTransitionTimeS );
    changeToTargetStateIfNecessary();
}

void LightBulb::setTargetState(const LightBulbState &rclState, const QDateTime& rclWhen)
{
    if ( !m_pclTargetState )
        m_pclTargetState = std::make_unique<LightBulbState>( rclState );
    else
        *m_pclTargetState = rclState;
    m_clTargetStateTimepoint = rclWhen;
    m_bIsInTargetState = getCurrentState() == rclState;
    qDebug() << name() << "target state"<< (m_bIsInTargetState ? "is already" : "should be") <<rclState<<"in"<<QDateTime::currentDateTime().msecsTo( rclWhen )/1000.0 << "seconds";
}

void LightBulb::setTargetState( const LightBulbState& rclState, float fSecondsInTheFuture )
{
    setTargetState( rclState, QDateTime::currentDateTime().addMSecs( fSecondsInTheFuture * 1000.f ) );
}

void LightBulb::onFalseState()
{
    if (m_bIsInTargetState) // false alarm...
        return;

    qDebug() << QString( (sender() == this || sender() == nullptr) ? "  -->" : name() ).toStdString().c_str() << "in false state: "<<getCurrentState()<<", should be:"<<getTargetState();

    int i_milliseconds_to_target_state = QDateTime::currentDateTime().msecsTo( m_clTargetStateTimepoint );
    // check if we should have reached our state already anyway...
    if ( i_milliseconds_to_target_state < -3000 ) // ok... this is longer than a second ago.. we don't care any more...
    {
        qDebug() << "      state activation is in far past ("<<i_milliseconds_to_target_state<<"ago)";
        getCurrentState() = getTargetState();
        m_bIsInTargetState = true;
        emit stateChanged();
    }
    else if ( i_milliseconds_to_target_state > -250 ) // we are in the first 0.25s... set a timer and wait
    {
        qDebug() << "      state activation is recent past. setting a timer";
        m_clStateLostGracePeriod.setSingleShot( true );
        m_clStateLostGracePeriod.start();
    }
    else //ok... now its time to actually do something...
    {
        LightBulbState cl_diff = getTargetState() - getCurrentState();
        qDebug() << "      PANIC! --> sending diff" << cl_diff;
        putStateOnLightBulb( cl_diff );
    }
}
