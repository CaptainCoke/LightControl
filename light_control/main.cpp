#include <iostream>
#include <QCoreApplication>
#include <QSettings>
#include "GatewayAccess.h"
#include "RemoteControlService.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Christopher Schwartz");
    QCoreApplication::setOrganizationDomain("christopherschwartz.de");
    QCoreApplication::setApplicationName("LightControl");

    QCoreApplication a(argc, argv);

    QSettings cl_settings;
    QString str_address, str_api_key;
    QVariant cl_address = cl_settings.value( "gateway/address" );
    QVariant cl_api_key = cl_settings.value( "gateway/api_key" );
    if ( cl_address.isValid() && cl_api_key.isValid() )
    {
        str_address = cl_address.toString();
        str_api_key = cl_api_key.toString();
    }
    else
    {
        std::cout << "please setup the gateway access in LightControlGui first" << std::endl;
        return 1;
    }

    GatewayAccess::instance().setConnection( str_address, str_api_key );

    RemoteControlService s;
    s.start();
    return a.exec();
}