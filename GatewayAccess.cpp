#include "GatewayAccess.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GatewayAccess::GatewayAccess()
: m_pclNetwork(std::make_unique<QNetworkAccessManager>())
{

}

GatewayAccess::~GatewayAccess() = default;

void GatewayAccess::setConnection(const QString &strGateWayAddress, const QString &strApiKey)
{
    m_strBaseUrl = QString("http://%1/api/%2/").arg(strGateWayAddress,strApiKey);
}

GatewayAccess& GatewayAccess::instance()
{
    static GatewayAccess s_singleton;
    return s_singleton;
}

QString GatewayAccess::requestApiKey( const QString& strGatewayAddress )
{
    m_strBaseUrl = QString("http://%1/api").arg(strGatewayAddress);
    QNetworkReply* pcl_reply = m_pclNetwork->post( createApiRequest(""), QByteArray("{ \"devicetype\": \"LightControl\" }") );

    // wait for reply to finish
    while ( !pcl_reply->isFinished() )
        QApplication::processEvents();
    pcl_reply->deleteLater();

    // parse result and return api key
    QJsonDocument cl_doc = getJSONFromReply( pcl_reply );

    return cl_doc.array()[0].toObject().value("success").toObject().value("username").toString();
}

QNetworkRequest GatewayAccess::createApiRequest(const QString &strSubURL) const
{
    return QNetworkRequest(QUrl(m_strBaseUrl+strSubURL));
}

void GatewayAccess::get(const QString& strRequest, std::function<void(const QJsonObject&)> fnParseReply)
{
    QNetworkReply* pcl_reply = m_pclNetwork->get( createApiRequest(strRequest) );
    connect( pcl_reply, &QNetworkReply::finished, [this,fnParseReply,pcl_reply](){
        QJsonDocument cl_doc = getJSONFromReply( pcl_reply );
        pcl_reply->deleteLater();
        fnParseReply(cl_doc.object());
    } );
}

void GatewayAccess::put(const QString &strRequest, const QByteArray& rclData, std::function<void (const QJsonArray &)> fnParseReply)
{
    QNetworkReply* pcl_reply = m_pclNetwork->put( createApiRequest(strRequest), rclData );
    connect( pcl_reply, &QNetworkReply::finished, [this,fnParseReply,pcl_reply](){
        QJsonDocument cl_doc = getJSONFromReply( pcl_reply );
        pcl_reply->deleteLater();
        fnParseReply(cl_doc.array());
    } );
}

QJsonDocument GatewayAccess::getJSONFromReply(QNetworkReply* pclReply)
{
    // check for error
    switch ( pclReply->error() )
    {
    case QNetworkReply::NoError:
        return QJsonDocument::fromJson( pclReply->readAll() );
        break;
    case QNetworkReply::OperationCanceledError:
        emit networkInfo( QString("Network reply to %1 was canceled").arg(pclReply->url().toString()) );
        break;
    default:
        emit networkError( QString("Network reply to %1 received error: %2").arg(pclReply->url().toString(),pclReply->errorString()) );
        break;
    }
    return QJsonDocument();
}
