#include "LightBulbWidget.h"
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>
#include "RGBLightBulb.h"
#include "CTLightBulb.h"
#include "RGBLightBulbWidget.h"
#include "CTLightBulbWidget.h"

LightBulbWidget::LightBulbWidget( const std::shared_ptr<LightBulb>& pclLight, QWidget *parent)
: DeviceNodeWidget(parent)
{
    setNode( pclLight );
}

LightBulbWidget::~LightBulbWidget() = default;

void LightBulbWidget::createGui()
{
    DeviceNodeWidget::createGui();
    auto pcl_light = getNode<LightBulb>();

    m_pclOnCheck = new QCheckBox("On", this);
    connect( m_pclOnCheck, &QCheckBox::toggled, pcl_light.get(), &LightBulb::setOn );
    addControl( "", m_pclOnCheck );

    m_pclBrightnessSlider = new QSlider(Qt::Horizontal,this);
    m_pclBrightnessSlider->setRange( 0, 255 );
    QLabel*     pcl_brightness_label  = new QLabel("0",this);
    QBoxLayout* pcl_brightness_layout = new QHBoxLayout;

    pcl_brightness_label->setMinimumWidth(40);
    pcl_brightness_layout->addWidget( m_pclBrightnessSlider );
    pcl_brightness_layout->addWidget( pcl_brightness_label );
    connect( m_pclBrightnessSlider, &QAbstractSlider::valueChanged, [pcl_brightness_label](int iValue){ pcl_brightness_label->setText( QString::number(iValue) ); } );
    connect( m_pclBrightnessSlider, &QAbstractSlider::sliderReleased, this, &LightBulbWidget::setLightBrightness );

    addControl( "Brightness", pcl_brightness_layout );
}



void LightBulbWidget::updateState()
{
    DeviceNodeWidget::updateState();
    auto pcl_light = getNode<LightBulb>();
    m_pclOnCheck->setChecked(        pcl_light->isOn() );
    m_pclBrightnessSlider->setValue( pcl_light->brightness() );
}

void LightBulbWidget::setLightBrightness()
{
    auto pcl_light = getNode<LightBulb>();
    if ( pcl_light )
        pcl_light->setBrightness( static_cast<uint8_t>(m_pclBrightnessSlider->value()) );
}

LightBulbWidget* LightBulbWidget::createWidget(const std::shared_ptr<LightBulb>& pclLight)
{
    LightBulbWidget* pcl_widget;
    if ( std::dynamic_pointer_cast<RGBLightBulb>(pclLight) )
        pcl_widget = new RGBLightBulbWidget(pclLight);
    else if ( std::dynamic_pointer_cast<CTLightBulb>(pclLight) )
        pcl_widget = new CTLightBulbWidget(pclLight);
    else
        pcl_widget = new LightBulbWidget(pclLight);

    pcl_widget->createGui();
    pcl_widget->update();
    return pcl_widget;
}
