#ifndef REMOTECONTROLSERVICE_H
#define REMOTECONTROLSERVICE_H

#include <QObject>

class RemoteControlService : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControlService(QObject *parent = nullptr);
    ~RemoteControlService() override;

    bool started() const;

public slots:
    void start();
    void stop();

private:
    void connectRemotesToGroups();
    void updateFullState( const QJsonObject& mapState );
    bool m_bStarted = false;
};

#endif // REMOTECONTROLSERVICE_H
