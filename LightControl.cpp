#include "LightControl.h"
#include "ui_LightControl.h"
#include <QJsonObject>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QBoxLayout>
#include "Widgets/SensorWidget.h"
#include "Widgets/LightBulbWidget.h"
#include "Nodes/LightBulb.h"
#include "Nodes/Sensor.h"
#include "GatewayAccess.h"

LightControl::LightControl(QWidget *parent)
: QMainWindow(parent)
, m_pclUI(std::make_unique<Ui::LightControl>())
{
    m_pclUI->setupUi(this);
    m_pclUI->lightsWidget->setLayout( new QVBoxLayout() );
    m_pclUI->sensorsWidget->setLayout( new QVBoxLayout() );
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

void LightControl::updateSensors()
{
    GatewayAccess::instance().get( "sensors", [this](const QJsonObject& rclObject){ updateSensorWidgets(rclObject); } );
}

void LightControl::removeWidget(QString strUniqueId)
{
    auto it_widget = m_mapNodeWidgets.find(strUniqueId);
    if ( it_widget != m_mapNodeWidgets.end() )
    {
        QWidget* pcl_widget = it_widget->second;
        QWidget* pcl_parent_widget = dynamic_cast<QWidget*>(pcl_widget->parent());
        pcl_parent_widget->layout()->removeWidget(pcl_widget);
        delete pcl_widget;
        m_mapNodeWidgets.erase( it_widget );
    }
}

template<class NodeFactory,class WidgetFactory>
void LightControl::updateWidget(const QJsonObject& mapNodeData, QLayout* pclLayout)
{
    for ( auto it_node = mapNodeData.constBegin(); it_node != mapNodeData.constEnd(); ++it_node )
    {
        QJsonObject cl_node = it_node.value().toObject();
        auto pcl_node = NodeFactory::get(it_node.key());
        if ( pcl_node )
            pcl_node->setNodeData( cl_node );
        else
            pcl_node = NodeFactory::create( it_node.key(), cl_node );
        auto it_widget = m_mapNodeWidgets.find( pcl_node->uniqueId() );
        if ( it_widget == m_mapNodeWidgets.end() )
        {
            DeviceNodeWidget* pcl_widget = WidgetFactory::createWidget(pcl_node);
            pclLayout->addWidget(pcl_widget);
            m_mapNodeWidgets[pcl_node->uniqueId()] = pcl_widget;
        }
        else
        {
            it_widget->second->updateNode();
        }
        connect( pcl_node.get(), &DeviceNode::nodeDeleted, this, &LightControl::removeWidget );
    }
}

void LightControl::updateLightWidgets(const QJsonObject& mapLights)
{
    updateWidget<LightBulb,LightBulbWidget>(mapLights,m_pclUI->lightsWidget->layout());
}

void LightControl::updateSensorWidgets(const QJsonObject& mapSensors)
{
    updateWidget<Sensor,SensorWidget>(mapSensors,m_pclUI->sensorsWidget->layout());
}
