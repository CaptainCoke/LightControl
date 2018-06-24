#ifndef REMOTECONTROLSERVICE_H
#define REMOTECONTROLSERVICE_H

#include <QObject>
#include <QTimer>

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

signals:
    void lightsChanged();

protected slots:
    void refreshLightNodes();
    void refreshGroupNodes();

protected:
    void connectRemotesToGroups();
    void updateFullState( const QJsonObject& mapState );

private:
    QTimer m_clStatePollingTimer;
    bool m_bStarted = false;
};

#endif // REMOTECONTROLSERVICE_H
