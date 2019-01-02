#include "LightBulbWidget.h"
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>
#include <QTimer>
#include <Nodes/RGBLightBulb.h>
#include <Nodes/CTLightBulb.h>
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

    m_pclStateReachedLabel = new QLabel("");
    addControl( "", m_pclStateReachedLabel );
    connect( pcl_light.get(), &LightBulb::targetStateReached, [this]{ setToTargetStateReached(); } );
    connect( pcl_light.get(), &LightBulb::targetStateLost, [this]{ setToTargetStateLost(); } );
    connect( pcl_light.get(), &LightBulb::targetStateChanged, [this]{ setToTargetStateChanging(); } );
    setToTargetStateChanging();

    m_pclOnCheck = new QCheckBox("On", this);
    connect( m_pclOnCheck, &QCheckBox::toggled, this, &LightBulbWidget::setOn );
    m_pclLastPoweredLabel = new QLabel("");
    QBoxLayout* pcl_power_layout = new QHBoxLayout;
    pcl_power_layout->addWidget(m_pclOnCheck);
    pcl_power_layout->addWidget( new QLabel("Last powered: ") );
    pcl_power_layout->addWidget(m_pclLastPoweredLabel);
    addControl( "", pcl_power_layout );
    setLastPoweredText( QDateTime() );

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

    QTimer* pcl_refresh_last_powered_timer = new QTimer(this);
    pcl_refresh_last_powered_timer->setInterval(2000);
    pcl_refresh_last_powered_timer->setSingleShot(false);
    connect( pcl_refresh_last_powered_timer, &QTimer::timeout, this, &LightBulbWidget::updateLastPoweredText );
    pcl_refresh_last_powered_timer->start();
}

void LightBulbWidget::updateLastPoweredText()
{
    if ( auto pcl_light = getNode<LightBulb>() )
        setLastPoweredText( pcl_light->lastTimepointSeenPowered() );
    else
        setLastPoweredText( QDateTime() );
}

void LightBulbWidget::setLastPoweredText( const QDateTime& tLastPowered )
{
    if ( !tLastPowered.isValid() )
        m_pclLastPoweredLabel->setText( "never" );
    else
        m_pclLastPoweredLabel->setText( QString("%1s ago").arg( tLastPowered.secsTo( QDateTime::currentDateTime() ) ) );
}

void LightBulbWidget::updateState()
{
    DeviceNodeWidget::updateState();
    auto pcl_light = getNode<LightBulb>();
    m_pclOnCheck->setChecked( pcl_light->isOn() );
    setLastPoweredText( pcl_light->lastTimepointSeenPowered() );

    auto pcl_dimmable_light = getNode<DimmableLightBulb>();
    if ( pcl_dimmable_light ) {
        m_pclBrightnessSlider->setValue( pcl_dimmable_light->brightness() );
        m_pclBrightnessSlider->show();
    } else
        m_pclBrightnessSlider->hide();

    if ( pcl_light->isInTargetState() )
        setToTargetStateReached();
    else
        setToTargetStateChanging();
}

void LightBulbWidget::setToTargetStateReached()
{
    m_pclStateReachedLabel->setText( "ready" );
    m_pclStateReachedLabel->setStyleSheet( "QLabel { color: green; }" );
}

void LightBulbWidget::setToTargetStateLost()
{
    m_pclStateReachedLabel->setText( "lost" );
    m_pclStateReachedLabel->setStyleSheet( "QLabel { color: red; }" );
}

void LightBulbWidget::setToTargetStateChanging()
{
    m_pclStateReachedLabel->setText(  "working" );
    m_pclStateReachedLabel->setStyleSheet( "QLabel { color: orange; }" );
}

void LightBulbWidget::setLightBrightness()
{
    auto pcl_light = getNode<DimmableLightBulb>();
    if ( pcl_light )
        pcl_light->setBrightness( static_cast<uint8_t>(m_pclBrightnessSlider->value()) );
}

void LightBulbWidget::setOn(bool bOn)
{
    auto pcl_light = getNode<LightBulb>();
    if ( pcl_light )
        pcl_light->setOn( bOn );
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
    pcl_widget->updateNode();
    return pcl_widget;
}
