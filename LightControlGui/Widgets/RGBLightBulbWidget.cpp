#include "RGBLightBulbWidget.h"
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <Nodes/RGBLightBulb.h>
#include <LightTemperature.h>

void RGBLightBulbWidget::setLightHue()
{
    auto pcl_light = getNode<RGBLightBulb>();
    pcl_light->setHue(m_pclHueSlider->value());
}

void RGBLightBulbWidget::setLightSaturation()
{
    auto pcl_light = getNode<RGBLightBulb>();
    pcl_light->setSaturation(static_cast<uint8_t>(m_pclSaturationSlider->value()));
}

void RGBLightBulbWidget::setLightTemperature()
{
    auto pcl_light = getNode<RGBLightBulb>();
    pcl_light->setColor( LightColor::fromTemperature( LightTemperature::fromMired( static_cast<uint16_t>(m_pclTemperatureSlider->value())) ));
}

void RGBLightBulbWidget::createGui()
{
    LightBulbWidget::createGui();

    m_pclSaturationSlider = new QSlider(Qt::Horizontal,this);
    m_pclSaturationSlider->setRange( 0, 255 );
    QLabel*     pcl_saturation_label  = new QLabel("0",this);
    QBoxLayout* pcl_saturation_layout = new QHBoxLayout;
    pcl_saturation_label->setMinimumWidth(40);
    pcl_saturation_layout->addWidget( m_pclSaturationSlider );
    pcl_saturation_layout->addWidget( pcl_saturation_label );
    connect( m_pclSaturationSlider, &QAbstractSlider::valueChanged, [pcl_saturation_label](int iValue){ pcl_saturation_label->setText( QString::number(iValue) ); } );
    connect( m_pclSaturationSlider, &QAbstractSlider::sliderReleased, this, &RGBLightBulbWidget::setLightSaturation );

    m_pclHueSlider = new QSlider(Qt::Horizontal,this);
    m_pclHueSlider->setRange( 0, 360 );
    QLabel*     pcl_hue_label  = new QLabel("0",this);
    QBoxLayout* pcl_hue_layout = new QHBoxLayout;
    pcl_hue_label->setMinimumWidth(40);
    pcl_hue_layout->addWidget( m_pclHueSlider );
    pcl_hue_layout->addWidget( pcl_hue_label );
    connect( m_pclHueSlider, &QAbstractSlider::valueChanged, [pcl_hue_label](int iValue){ pcl_hue_label->setText( QString::number(iValue) ); } );
    connect( m_pclHueSlider, &QAbstractSlider::sliderReleased, this, &RGBLightBulbWidget::setLightHue );

    addControl( "Hue", pcl_hue_layout );
    addControl( "Saturation", pcl_saturation_layout );

    m_pclTemperatureSlider = new QSlider(Qt::Horizontal,this);
    m_pclTemperatureSlider->setRange( LightTemperature::fromKelvin(6500).mired(), LightTemperature::fromKelvin(2200).mired() );
    QLabel*     pcl_temperature_label  = new QLabel("0",this);
    QBoxLayout* pcl_temperature_layout = new QHBoxLayout;
    pcl_temperature_label->setMinimumWidth(40);
    pcl_temperature_layout->addWidget( m_pclTemperatureSlider );
    pcl_temperature_layout->addWidget( pcl_temperature_label );
    connect( m_pclTemperatureSlider, &QAbstractSlider::valueChanged, [pcl_temperature_label](int iValue){ pcl_temperature_label->setText( QString::number( LightTemperature::fromMired(iValue).kelvin())+"K" ); } );
    connect( m_pclTemperatureSlider, &QAbstractSlider::sliderReleased, this, &RGBLightBulbWidget::setLightTemperature );

    addControl( "Temperature", pcl_temperature_layout );
}

void RGBLightBulbWidget::updateState()
{
    LightBulbWidget::updateState();
    auto pcl_light = getNode<RGBLightBulb>();
    m_pclSaturationSlider->setValue( pcl_light->saturation() );
    m_pclHueSlider->setValue( pcl_light->hue() );
    m_pclTemperatureSlider->setValue( pcl_light->color().temperature().mired() );
}
