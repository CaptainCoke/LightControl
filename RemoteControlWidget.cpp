#include "RemoteControlWidget.h"
#include <QLabel>
#include "RemoteControl.h"


void RemoteControlWidget::createGui()
{
    SensorWidget::createGui();

    m_pclLastUpdatedLabel = new QLabel("never");
    m_pclButtonLabel = new QLabel("none");

    addControl( "Last updated", m_pclLastUpdatedLabel );
    addControl( "Last button", m_pclButtonLabel );

    m_pclLastUpdatedTimer = new QTimer(this);
    m_pclLastUpdatedTimer->start( 1000 );
    connect( m_pclLastUpdatedTimer, &QTimer::timeout, this, &RemoteControlWidget::updateLastUpdated );
}

static QString getNextTimeUnit( qint64& iTime, int iPeriod, const QString& strFormat )
{
    int i_interval = iTime % iPeriod;
    iTime = (iTime - i_interval)/iPeriod;
    return strFormat.arg(i_interval);
}

static QString secondsToHumanReadableTimeInterval(qint64 iTime)
{
    iTime = std::abs(iTime);
    QStringList lst_intervals;
    lst_intervals.push_front( getNextTimeUnit( iTime, 60, QString("%1 sec") ) );
    if ( iTime > 0 )
        lst_intervals.push_front( getNextTimeUnit( iTime, 60, QString("%1 min") ) );
    if ( iTime > 0 )
        lst_intervals.push_front( getNextTimeUnit( iTime, 24, QString("%1 hours") ) );
    if ( iTime > 0 )
        lst_intervals.push_front( QString("%1 days").arg(iTime) );
    QString str_interval = lst_intervals.back();
    lst_intervals.pop_back();
    if ( !lst_intervals.isEmpty() )
        str_interval.prepend( lst_intervals.join(", ")+" and " );
    return str_interval;
}

void RemoteControlWidget::updateLastUpdated()
{
    auto pcl_remote = getNode<RemoteControl>();
    qint64 i_seconds_ago = pcl_remote->lastUpdated().secsTo( QDateTime::currentDateTime() );
    m_pclLastUpdatedLabel->setText( QString( "%1 ago" ).arg(secondsToHumanReadableTimeInterval(i_seconds_ago)) );
}

void RemoteControlWidget::updateState()
{
    SensorWidget::updateState();
    auto pcl_remote = getNode<RemoteControl>();
    QString str_button, str_action;
    switch ( pcl_remote->button() )
    {
    case RemoteControl::Button::Power:    str_button = "Power";    break;
    case RemoteControl::Button::Brighter: str_button = "Brighter"; break;
    case RemoteControl::Button::Dimmer:   str_button = "Dimmer";   break;
    case RemoteControl::Button::Next:     str_button = "Next";     break;
    case RemoteControl::Button::Previous: str_button = "Previous"; break;
    default:
        str_button = QString::number(pcl_remote->buttonEvent());
    }
    switch ( pcl_remote->action() )
    {
    case RemoteControl::Action::Holding:  str_action = "Holding";  break;
    case RemoteControl::Action::Pressed:  str_action = "Pressed";  break;
    case RemoteControl::Action::Released: str_action = "Released"; break;
    default:
        str_action = QString::number(pcl_remote->buttonEvent());
    }
    m_pclButtonLabel->setText( str_action + " " + str_button );
    updateLastUpdated();
}

