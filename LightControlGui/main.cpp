#include <QApplication>
#include <QSettings>
#include <QInputDialog>
#include <QMessageBox>
#include "LightControl.h"
#include "GatewayAccess.h"
#include "NetworkService.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Christopher Schwartz");
    QCoreApplication::setOrganizationDomain("christopherschwartz.de");
    QCoreApplication::setApplicationName("LightControl");

    QApplication a(argc, argv);

    QSettings cl_settings;
    QString str_address;
    QVariant cl_address = cl_settings.value( "gateway/address" );
    if ( cl_address.isValid() )
        str_address = cl_address.toString();
    else
    {
        str_address = QInputDialog::getText( nullptr, "Initial Setup", "Please enter the gateway address (e.g. '192.168.0.2:80')" );
        if ( str_address.isEmpty() )
            return 1;
        cl_settings.setValue( "gateway/address", str_address );
    }

    QString str_api_key;
    QVariant cl_api_key = cl_settings.value( "gateway/api_key" );
    if ( cl_api_key.isValid() )
        str_api_key = cl_api_key.toString();
    else
    {
        QMessageBox::information( nullptr, "Initial Setup", "Please unlock the gateway and click OK within 60 seconds to obtain an API key" );
        str_api_key = GatewayAccess::instance().requestApiKey( str_address );
        if ( str_api_key.isEmpty() )
            return 1;
        cl_settings.setValue( "gateway/api_key", str_api_key );
    }

    GatewayAccess::instance().setConnection( str_address, str_api_key );

    LightControl gui;
    NetworkService network;
    QObject::connect( &network, &NetworkService::networkUp, &gui, &LightControl::updateWidgets );
    QObject::connect( &network, &NetworkService::networkDown, &gui, &LightControl::updateWidgets );

    network.start();
    gui.show();

    return a.exec();
}
