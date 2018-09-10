#ifndef ALARMSERVICE_H
#define ALARMSERVICE_H

#include <QObject>
#include <QTimer>

class AlarmService : public QObject
{
    Q_OBJECT
public:
    explicit AlarmService(QObject *parent = nullptr);
    ~AlarmService() override;

public slots:
    void start();
    void stop();

protected slots:
    void handleAlarms();
    void interruptAlarmsOnExternalChange();

protected:
    void loadAlarms();

private:
    class Alarm;
    std::list<Alarm> m_lstAlarms;
    QTimer m_clPeriodicCheckTimer;
};

#endif // ALARMSERVICE_H
