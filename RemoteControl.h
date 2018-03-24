#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QDateTime>
#include "Sensor.h"

class RemoteControl : public Sensor
{
public:
    ~RemoteControl() override = default;
    enum class Button {Power,Brighter,Dimmer,Previous,Next,Unknown};
    enum class Action {Pressed,Holding,Released,Unknown};

    const QDateTime& lastUpdated() const { return m_tLastUpdated; }
    Button button() const;
    Action action() const;
    int buttonEvent() const { return m_iButtonEvent; }

    static bool isRemoteControl( const QJsonObject &rclObject );

protected:
    using Sensor::Sensor;
    bool setStateData(const QJsonObject &rclObject) override;

private:
    QDateTime m_tLastUpdated;
    int m_iButtonEvent;
};

#endif // REMOTECONTROL_H
