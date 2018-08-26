#include "LightStateTransition.h"
#include <QtDebug>
#include "Nodes/RGBLightBulb.h"
#include "Nodes/CTLightBulb.h"

std::map<QString,std::shared_ptr<LightStateTransition>> LightStateTransition::s_mapAllLightStateTransitions;

LightStateTransition::LightStateTransition( std::shared_ptr<LightBulb> pclLight, double fTransitionTimeSeconds )
: m_pclLight(std::move(pclLight)), m_fTransitionTimeSeconds(fTransitionTimeSeconds)
{
}

void LightStateTransition::start()
{
    s_mapAllLightStateTransitions[m_pclLight->id()] = shared_from_this();

    qInfo() << "starting state transition";
    m_tStart = QDateTime::currentDateTime();
    m_clConnection = QObject::connect( m_pclLight.get(), &LightBulb::targetStateReached, [this]{ step(); } );
    m_bFinished = false;
    m_eNextAttribute = static_cast<LightAttribute>(NUM_ATTRIBUTES-1);
    m_clNextEventTimer.setSingleShot(true);
    m_clNextEventTimer.setInterval(0);
    QObject::connect( &m_clNextEventTimer, &QTimer::timeout, [this]{ setValueAtPosition(0.0); } );
    m_clNextEventTimer.start();
}

void LightStateTransition::abort()
{
    qInfo() << "aborted";
    m_clNextEventTimer.stop();
    m_clNextEventTimer.disconnect();
    cleanup();
}

bool LightStateTransition::isFinished()
{
    return m_bFinished;
}

void LightStateTransition::abortIfExists(const QString &strLightId)
{
    if ( auto it_light = s_mapAllLightStateTransitions.find( strLightId ); it_light != s_mapAllLightStateTransitions.end() )
    {
        it_light->second->abort();
    }
}

void LightStateTransition::step()
{
    double f_step_time_sec = m_tStart.msecsTo( QDateTime::currentDateTime() ) / 1000.0;
    double f_alpha = f_step_time_sec/m_fTransitionTimeSeconds;
    setValueAtPosition(f_alpha);
}

void LightStateTransition::finish()
{
    qInfo() << "finished";
    cleanup();
    m_bFinished = true;
}

void LightStateTransition::cleanup()
{
    QObject::disconnect( m_clConnection );
    if ( auto it_this = s_mapAllLightStateTransitions.find( m_pclLight->id() ); it_this != s_mapAllLightStateTransitions.end() )
    {
        s_mapAllLightStateTransitions.erase(it_this);
    }
}

void LightStateTransition::setValueAtPosition( double fAlpha )
{
    fAlpha = std::clamp(fAlpha,0.0,1.0);
    m_eNextAttribute = static_cast<LightAttribute>((m_eNextAttribute + 1) % NUM_ATTRIBUTES);
    switch( m_eNextAttribute )
    {
    case Power:
        if ( !m_pclLight->isOn() )
        {
            qInfo() << fAlpha*100 << "%  --> power on";
            m_pclLight->setOn(true);
            break;
        }
        [[fallthrough]];
    case Brightness:
    {
        uint8_t brightness_step = static_cast<uint8_t>(std::clamp(m_fBrightnessStart*(1.0-fAlpha) + m_fBrightnessEnd*fAlpha, 1.0, 255.0));
        if ( m_pclLight->brightness() != brightness_step)
        {
            qInfo() << fAlpha*100 << "%  --> brightness:" << brightness_step;
            m_pclLight->setBrightness( brightness_step );
            break;
        }
        [[fallthrough]];
    }
    case Temperature:
    {
        double mired_step = std::clamp( m_fMiredStart*(1.0-fAlpha) + m_fMiredEnd*fAlpha, m_fMinMired, m_fMaxMired);
        auto cl_next_temperature = LightTemperature::fromMired( static_cast<uint16_t>( mired_step ) );
        auto cl_next_color = LightColor::fromTemperature( cl_next_temperature );
        if ( auto pcl_rgb_light = std::dynamic_pointer_cast<RGBLightBulb>( m_pclLight ); pcl_rgb_light && pcl_rgb_light->color() != cl_next_color )
        {
            qInfo() << fAlpha*100 << "%  --> color:" << mired_step << " ="<<cl_next_color.x() <<cl_next_color.y();
            pcl_rgb_light->setColor( cl_next_color );
            break;
        }
        else if ( auto pcl_ct_light = std::dynamic_pointer_cast<CTLightBulb>( m_pclLight ); pcl_ct_light && pcl_ct_light->temperature() != cl_next_temperature )
        {
            qInfo() << fAlpha*100 << "%  --> temperature:" << mired_step << " = "<<cl_next_temperature.kelvin() << "K";
            pcl_ct_light->setTemperature( cl_next_temperature );
            break;
        }
        [[fallthrough]];
    }
    default:
        if ( fAlpha >= 1.0 )
            finish();
        else // apparently we are alreay in the state for this position... set a timer to try again later
        {
            m_clNextEventTimer.setSingleShot(true);
            m_clNextEventTimer.setInterval(50);
            QObject::connect( &m_clNextEventTimer, &QTimer::timeout, [this]{ step(); } );
            m_clNextEventTimer.start();
            qInfo() << fAlpha*100 << "% --> all values already as inteded... setting timer";
        }
    }
}