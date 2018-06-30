#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QTimer>

class NetworkService : public QObject
{
    Q_OBJECT
public:
    explicit NetworkService(QObject *parent = nullptr);
    ~NetworkService() override;

    bool isUp() const;

public slots:
    void start();
    void stop();

signals:
    void lightsChanged();
    void networkUp();
    void networkDown();

protected slots:
    void refreshLightNodes();
    void refreshGroupNodes();

protected:
    void updateFullState( const QJsonObject& mapState );

private:
    QTimer m_clStatePollingTimer;
    bool m_bIsUp = false;
};

#endif // NETWORKSERVICE_H
