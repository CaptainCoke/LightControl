#include "GatewayAccess.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCoreApplication>
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
        QCoreApplication::processEvents();
    pcl_reply->deleteLater();

    // parse result and return api key
    QJsonDocument cl_doc = getJSONFromReplyIfSuccesfull( pcl_reply );

    return cl_doc.array()[0].toObject().value("success").toObject().value("username").toString();
}

QUrl GatewayAccess::getAddress() const
{
    return QUrl( m_strBaseUrl );
}

QNetworkRequest GatewayAccess::createApiRequest(const QString &strSubURL, const QString& strEtag) const
{
    QNetworkRequest cl_request(QUrl(m_strBaseUrl+strSubURL));
    if ( !strEtag.isEmpty() )
        cl_request.setRawHeader( QByteArray("If-None-Match"), QByteArray( ("\""+strEtag+"\"").toLocal8Bit() ) );
    return cl_request;
}

static QJsonDocument getJSONFromReply(QNetworkReply* pclReply)
{
    return QJsonDocument::fromJson( pclReply->readAll() );
}

void GatewayAccess::get(const QString& strRequest, std::function<void(const QJsonObject&)> fnParseReply, const QString& strEtag)
{
    QNetworkReply* pcl_reply = m_pclNetwork->get( createApiRequest(strRequest, strEtag) );
    connect( pcl_reply, &QNetworkReply::finished, [this,fnParseReply,pcl_reply](){
        pcl_reply->deleteLater();
        if ( networkReplySuccessful(pcl_reply) )
        {
            QJsonDocument cl_doc = getJSONFromReply( pcl_reply );
            fnParseReply(cl_doc.object());
        }
    } );
}

void GatewayAccess::put(const QString &strRequest, const QByteArray& rclData, std::function<void (const QJsonArray &)> fnParseReply)
{
    QNetworkReply* pcl_reply = m_pclNetwork->put( createApiRequest(strRequest), rclData );
    connect( pcl_reply, &QNetworkReply::finished, [this,fnParseReply,pcl_reply](){
        QJsonDocument cl_doc = getJSONFromReplyIfSuccesfull( pcl_reply );
        pcl_reply->deleteLater();
        fnParseReply(cl_doc.array());
    } );
}

void GatewayAccess::del(const QString &strRequest, std::function<void()> fnNotifySuccess)
{
    QNetworkReply* pcl_reply = m_pclNetwork->deleteResource( createApiRequest(strRequest) );
    connect( pcl_reply, &QNetworkReply::finished, [this,fnNotifySuccess,pcl_reply](){
        QJsonDocument cl_doc = getJSONFromReplyIfSuccesfull( pcl_reply );
        pcl_reply->deleteLater();
        fnNotifySuccess();
    } );
}

QJsonDocument GatewayAccess::getJSONFromReplyIfSuccesfull(QNetworkReply* pclReply)
{
    if ( networkReplySuccessful( pclReply ) )
        return getJSONFromReply(pclReply);
    else
        return QJsonDocument();
}

bool GatewayAccess::networkReplySuccessful( QNetworkReply* pclReply )
{
    // check for error
    int i_status = pclReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString str_original_request = getApiRequestFromURL(pclReply->url());
    switch ( pclReply->error() )
    {
    case QNetworkReply::NoError: // check HTTP status code of reply
        switch ( i_status )
        {
        case 200: // 200 OK
            //emit networkInfo( QString("Server succeeded with request %1").arg(str_original_request) );
            return true;
        case 304: // 304 NOT MODIFIED
            //emit networkInfo( QString("Server indicated no modification for request %1").arg(str_original_request) );
            return false;
        default:
            emit networkInfo( QString("Server responded with status code %1 for request %2").arg(i_status).arg(str_original_request) );
            return false;
        }
    case QNetworkReply::OperationCanceledError:
        emit networkInfo( QString("Network reply to \"%1\" was canceled").arg(str_original_request) );
        return false;
    case QNetworkReply::ConnectionRefusedError:
        emit connectionRefused();
        [[fallthrough]];
    default:
        emit networkError( QString("Network reply to \"%1\" received error: %2").arg(str_original_request,pclReply->errorString()) );
        return false;
    }
}

QString GatewayAccess::getApiRequestFromURL( const QUrl& rclUrl )
{
    QStringList lst_request_elements = rclUrl.path().split( "/", QString::SkipEmptyParts );
    // remove the "api" and api key elements from the request
    lst_request_elements.pop_front();
    lst_request_elements.pop_front();
    return lst_request_elements.join("/");
}
