#include "CTLightBulbWidget.h"
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include "CTLightBulb.h"

void CTLightBulbWidget::setLightTemperature()
{
    auto pcl_light = getNode<CTLightBulb>();
    pcl_light->setTemperature(m_pclTemperatureSlider->value());
}

void CTLightBulbWidget::createGui()
{
    LightBulbWidget::createGui();

    m_pclTemperatureSlider = new QSlider(Qt::Horizontal,this);
    m_pclTemperatureSlider->setRange( 0, 255 );
    QLabel*     pcl_temperature_label  = new QLabel("0",this);
    QBoxLayout* pcl_temperature_layout = new QHBoxLayout;
    pcl_temperature_label->setMinimumWidth(40);
    pcl_temperature_layout->addWidget( m_pclTemperatureSlider );
    pcl_temperature_layout->addWidget( pcl_temperature_label );
    connect( m_pclTemperatureSlider, &QAbstractSlider::valueChanged, [pcl_temperature_label](int iValue){ pcl_temperature_label->setText( QString::number(iValue) ); } );
    connect( m_pclTemperatureSlider, &QAbstractSlider::sliderReleased, this, &CTLightBulbWidget::setLightTemperature );

    addControl( "Temperature", pcl_temperature_layout );
}

void CTLightBulbWidget::updateState()
{
    LightBulbWidget::updateState();
    auto pcl_light = getNode<CTLightBulb>();
    m_pclTemperatureSlider->setRange( pcl_light->minTemperature(), pcl_light->maxTemperature() );
    m_pclTemperatureSlider->setValue( pcl_light->temperature() );
}
