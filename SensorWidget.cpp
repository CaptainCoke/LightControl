#include "SensorWidget.h"
#include <QProgressBar>
#include <QLabel>
#include <QCheckBox>
#include "Sensor.h"
#include "RemoteControl.h"
#include "RemoteControlWidget.h"

SensorWidget::SensorWidget(const std::shared_ptr<Sensor>& pclSensor, QWidget *parent)
: DeviceNodeWidget(parent)
{
    setNode( pclSensor );
}

SensorWidget::~SensorWidget() = default;

void SensorWidget::createGui()
{
    DeviceNodeWidget::createGui();
    auto pcl_sensor = getNode<Sensor>();

    m_pclOnCheck = new QCheckBox("On", this);
    m_pclOnCheck->setEnabled(false);
    addControl( "", m_pclOnCheck );

    m_pclBatteryBar = new QProgressBar(this);
    m_pclBatteryBar->setRange(0,100);
    addControl( "Battery", m_pclBatteryBar );
}


void SensorWidget::updateState()
{
    DeviceNodeWidget::updateState();
    auto pcl_sensor = getNode<Sensor>();
    m_pclOnCheck->setChecked( pcl_sensor->isOn() );
    m_pclBatteryBar->setValue( pcl_sensor->battery() );
}

SensorWidget* SensorWidget::createWidget(const std::shared_ptr<Sensor>& pclSensor)
{
    SensorWidget* pcl_widget;
    if ( std::dynamic_pointer_cast<RemoteControl>(pclSensor) )
        pcl_widget = new RemoteControlWidget(pclSensor);
    else
        pcl_widget = new SensorWidget(pclSensor);

    pcl_widget->createGui();
    pcl_widget->updateNode();
    return pcl_widget;
}
