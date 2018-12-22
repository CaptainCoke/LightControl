#include "CTLightBulbWidget.h"
#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include <QSpinBox>
#include <Nodes/CTLightBulb.h>

void CTLightBulbWidget::setLightTemperature()
{
    auto pcl_light = getNode<CTLightBulb>();
    pcl_light->setTemperature( LightTemperature::fromMired(m_pclTemperatureSpinBox->value()));
}

void CTLightBulbWidget::updateValueFromInput(int iMired)
{
    m_pclTemperatureLabel->setText( QString::number( LightTemperature::fromMired(iMired).kelvin())+"K" );
    QSignalBlocker cl_block2( m_pclTemperatureSpinBox ), cl_block( m_pclTemperatureSlider );
    m_pclTemperatureSpinBox->setValue( iMired );
    m_pclTemperatureSlider->setValue( iMired );
}

void CTLightBulbWidget::createGui()
{
    LightBulbWidget::createGui();

    m_pclTemperatureSlider = new QSlider(Qt::Horizontal,this);
    m_pclTemperatureSlider->setRange( LightTemperature::fromKelvin(6500).mired(), LightTemperature::fromKelvin(2200).mired() );
    m_pclTemperatureLabel = new QLabel("0",this);
    m_pclTemperatureLabel->setMinimumWidth(40);
    m_pclTemperatureSpinBox = new QSpinBox(this);
    m_pclTemperatureSpinBox->setRange( m_pclTemperatureSlider->minimum(), m_pclTemperatureSlider->maximum() );
    m_pclTemperatureSpinBox->setSuffix( " mired" );

    connect( m_pclTemperatureSlider, &QAbstractSlider::valueChanged, this, &CTLightBulbWidget::updateValueFromInput );
    connect( m_pclTemperatureSlider, &QAbstractSlider::sliderReleased, this, &CTLightBulbWidget::setLightTemperature );
    connect( m_pclTemperatureSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int iMired){
        updateValueFromInput(iMired);
        setLightTemperature();
    });

    QGridLayout* pcl_temperature_layout = new QGridLayout;
    pcl_temperature_layout->addWidget( m_pclTemperatureSlider, 0, 0, 1, 1 );
    pcl_temperature_layout->addWidget( m_pclTemperatureLabel,  0, 1, 1, 1 );
    pcl_temperature_layout->addWidget( m_pclTemperatureSpinBox, 1, 0, 1, 1 );

    addControl( "Temperature", pcl_temperature_layout );
}

void CTLightBulbWidget::updateState()
{
    LightBulbWidget::updateState();
    auto pcl_light = getNode<CTLightBulb>();
    m_pclTemperatureSlider->setRange( pcl_light->minTemperature().mired(), pcl_light->maxTemperature().mired() );
    m_pclTemperatureSlider->setValue( pcl_light->temperature().mired() );
    QSignalBlocker cl_block2( m_pclTemperatureSpinBox );
    m_pclTemperatureSpinBox->setRange( m_pclTemperatureSlider->minimum(), m_pclTemperatureSlider->maximum() );
    m_pclTemperatureSpinBox->setValue( pcl_light->temperature().mired() );
}
