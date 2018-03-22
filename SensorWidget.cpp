#include "SensorWidget.h"
#include <QProgressBar>
#include <QLabel>
#include <QCheckBox>
#include "Sensor.h"

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
    connect( pcl_sensor.get(), SIGNAL(stateChanged()), this, SLOT(updateState()) );

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
    pcl_widget = new SensorWidget(pclSensor);

    pcl_widget->createGui();
    pcl_widget->update();
    return pcl_widget;
}
