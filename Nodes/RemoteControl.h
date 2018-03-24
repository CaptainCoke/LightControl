#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QDateTime>
#include "Sensor.h"

class RemoteControl : public Sensor
{
    Q_OBJECT
public:
    ~RemoteControl() override = default;
    enum class Button {Power,Brighter,Dimmer,Previous,Next,Unknown};
    enum class Action {Pressed,Holding,Released,Unknown};

    qint64 secondsSinceLastUpdate() const;
    const QDateTime& lastUpdated() const { return m_tLastUpdated; }
    Button button() const;
    Action action() const;
    int buttonEvent() const { return m_iButtonEvent; }
    const QString& group() const { return m_strGroup; }

    static bool isRemoteControl( const QJsonObject &rclObject );

signals:
    void buttonPressed(Button);
    void buttonReleased(Button);
    void buttonHeld(Button);

protected:
    using Sensor::Sensor;
    bool setStateData(const QJsonObject &rclObject) override;
    bool setConfigData(const QJsonObject &rclObject) override;

private:
    bool signalButtonEvent(int iButtonEvent);

    QDateTime m_tLastUpdated;
    int m_iButtonEvent;
    QString m_strGroup;
};

#endif // REMOTECONTROL_H
