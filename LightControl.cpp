#include "LightControl.h"
#include "ui_LightControl.h"
#include <QJsonObject>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QBoxLayout>
#include "LightBulbWidget.h"
#include "LightBulb.h"
#include "GatewayAccess.h"

LightControl::LightControl(QWidget *parent)
: QMainWindow(parent)
, m_pclUI(std::make_unique<Ui::LightControl>())
{
    m_pclUI->setupUi(this);
    m_pclUI->centralWidget->setLayout( new QVBoxLayout() );
    connect( &GatewayAccess::instance(), &GatewayAccess::networkInfo, [this](const QString& strMessage){
        m_pclUI->statusBar->showMessage(strMessage);
    } );
    connect( &GatewayAccess::instance(), &GatewayAccess::networkError, [this](const QString& strMessage){
        m_pclUI->statusBar->showMessage(strMessage);
        QMessageBox::critical(this, "Network Error", strMessage);
    } );
}

LightControl::~LightControl() = default;

void LightControl::updateLights()
{
    GatewayAccess::instance().get( "lights", [this](const QJsonObject& rclObject){ updateLightWidgets(rclObject); } );
}

void LightControl::updateLightWidgets(const QJsonObject& mapLights)
{
    for ( auto it_light = mapLights.constBegin(); it_light != mapLights.constEnd(); ++it_light )
    {
        QJsonObject cl_light = it_light.value().toObject();
        std::shared_ptr<LightBulb> pcl_light = LightBulb::get(it_light.key());
        if ( pcl_light )
            pcl_light->setLightData( cl_light );
        else
            pcl_light = LightBulb::create( it_light.key(), cl_light );
        auto it_widget = m_mapLightWidgets.find( it_light.key() );
        if ( it_widget == m_mapLightWidgets.end() )
        {
            QLayout* pcl_layout = m_pclUI->centralWidget->layout();
            LightBulbWidget* pcl_widget = LightBulbWidget::createWidget(pcl_light);
            pcl_layout->addWidget(pcl_widget);
            m_mapLightWidgets[it_light.key()] = pcl_widget;
        }
        else
        {
            it_widget->second->update();
        }
    }
}
