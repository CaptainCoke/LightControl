#ifndef REMOTECONTROLSERVICE_H
#define REMOTECONTROLSERVICE_H

#include <QObject>
#include <Nodes/RemoteControl.h>

class LightGroup;
class RemoteControlButtonHandler;

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
    void handleButtonPress( RemoteControl::Button button, LightGroup& rclGroup );

private:
    std::map<RemoteControl::Button,std::shared_ptr<RemoteControlButtonHandler>> m_mapButtonHandler;
};

#endif // REMOTECONTROLSERVICE_H
