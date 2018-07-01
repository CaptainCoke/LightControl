#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QTimer>
#include <memory>
#include <functional>

class QWebSocket;

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
    void refreshWebsocketPort();
    void handlePushMessage( const QString& strMessage );

protected:
    void updateFullState( const QJsonObject& mapState );
    void updateWebsocketPort( const QJsonObject& rclConfig );

private:
    QTimer m_clStatePollingTimer;
    std::unique_ptr<QWebSocket> m_pclWebSocket;
    std::map<QString,std::function<void(const QJsonObject&)>> m_mapPushMessageHandlers;
    bool m_bIsUp = false;
};

#endif // NETWORKSERVICE_H
