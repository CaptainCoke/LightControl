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

private:
    void connectRemotesToGroups();
    void updateFullState( const QJsonObject& mapState );
};

#endif // REMOTECONTROLSERVICE_H
