#ifndef GATEWAYACCESS_H
#define GATEWAYACCESS_H

#include <memory>
#include <functional>
#include <QObject>
#include <QString>

class QNetworkRequest;
class QNetworkReply;
class QNetworkAccessManager;

class GatewayAccess : public QObject
{
    Q_OBJECT

public:
    ~GatewayAccess();

    void setConnection( const QString& strGatewayAddress, const QString& strApiKey );

    void get( const QString& strRequest, std::function<void(const QJsonObject&)> fnParseReply );
    void put( const QString& strRequest, const QByteArray &rclData, std::function<void(const QJsonArray&)> fnParseReply );

    static GatewayAccess& instance();

    QString requestApiKey(const QString& strGatewayAddress);

signals:
    void networkInfo(const QString&);
    void networkError(const QString&);

private:
    GatewayAccess();
    QNetworkRequest createApiRequest( const QString& strSubURL ) const;
    QJsonDocument getJSONFromReply(QNetworkReply* pclReply);
    std::unique_ptr<QNetworkAccessManager> m_pclNetwork;
    QString m_strBaseUrl;
};

#endif // GATEWAYACCESS_H
