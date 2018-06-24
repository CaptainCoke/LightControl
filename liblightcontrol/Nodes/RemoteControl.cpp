#include "RemoteControl.h"
#include <QJsonObject>

bool RemoteControl::isRemoteControl(const QJsonObject &rclObject)
{
    if ( rclObject.value("type").toString() == "ZHASwitch" )
        return true;
    return false;
}

qint64 RemoteControl::secondsSinceLastUpdate() const
{
    return lastUpdated().secsTo( QDateTime::currentDateTime() );
}

RemoteControl::Button RemoteControl::button() const
{
    // button codes of TRADFRI remote control
    switch (m_iButtonEvent - (m_iButtonEvent % 10)) {
    case 1000: return Button::Power;
    case 2000: return Button::Brighter;
    case 3000: return Button::Dimmer;
    case 4000: return Button::Previous;
    case 5000: return Button::Next;
    default:   return Button::Unknown;
    }
}

RemoteControl::Action RemoteControl::action() const
{
    // action codes of TRADFRI remote control
    switch (m_iButtonEvent % 10) {
    case 1:  return Action::Holding;
    case 2:  return Action::Pressed;
    case 3:  return Action::Released;
    default: return Action::Unknown;
    }
}

bool RemoteControl::setStateData(const QJsonObject &rclObject)
{
    bool b_changed = Sensor::setStateData(rclObject);
    QString str_last_updated = rclObject.value("lastupdated").toString();
    QDateTime t_last_updated = QDateTime::fromString(str_last_updated,Qt::ISODate);
    t_last_updated.setTimeSpec(Qt::UTC);
    int i_button_event = rclObject.value("buttonevent").toInt();
    b_changed = t_last_updated != m_tLastUpdated;
    m_tLastUpdated = t_last_updated;
    signalButtonEvent(i_button_event);
    return b_changed;
}

bool RemoteControl::setConfigData(const QJsonObject &rclObject)
{
    bool b_changed = Sensor::setConfigData(rclObject);
    QString str_group = rclObject.value("group").toString();
    b_changed = b_changed || str_group != m_strGroup;
    m_strGroup = str_group;
    return b_changed;
}

void RemoteControl::setNodeData(const QJsonObject &rclObject)
{
    Sensor::setNodeData( rclObject );
    refreshPeriodically(200);
}

bool RemoteControl::signalButtonEvent(int iButtonEvent)
{
    bool b_changed = iButtonEvent == m_iButtonEvent;
    m_iButtonEvent = iButtonEvent;

    switch( action() ){
    case Action::Holding:  emit buttonHeld(button());     break;
    case Action::Pressed:  emit buttonPressed(button());  break;
    case Action::Released: emit buttonReleased(button()); break;
    default: // no signal for any other case...
        break;
    }

    return b_changed;
}
