#ifndef REMOTECONTROLSERVICE_H
#define REMOTECONTROLSERVICE_H

#include <QObject>

class RemoteControlService : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControlService(QObject *parent = nullptr);
    ~RemoteControlService() override;

public slots:
    void start();
    void stop();

protected:
    void connectRemotesToGroups();
};

#endif // REMOTECONTROLSERVICE_H
