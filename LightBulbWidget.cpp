#include "LightBulbWidget.h"
#include "ui_LightBulbWidget.h"
#include "RGBLightBulb.h"
#include "CTLightBulb.h"
#include "RGBLightBulbWidget.h"
#include "CTLightBulbWidget.h"

LightBulbWidget::LightBulbWidget( const std::shared_ptr<LightBulb>& pclLight, QWidget *parent)
: QWidget(parent)
, m_pclUI(std::make_unique<Ui::LightBulbWidget>())
, m_pclLight( pclLight )
{
}

LightBulbWidget::~LightBulbWidget() = default;

void LightBulbWidget::createGui()
{
    auto pcl_light = getLight();
    m_pclUI->setupUi(this);
    connect( m_pclUI->checkOn, SIGNAL(toggled(bool)), pcl_light.get(), SLOT(setOn(bool)) );
    connect( m_pclUI->buttonRefresh, SIGNAL(clicked()), pcl_light.get(), SLOT(refreshState()) );
    connect( m_pclUI->sliderBrightness, &QAbstractSlider::valueChanged, [this](int iValue){ m_pclUI->labelBrightness->setText( QString::number(iValue) ); } );
    connect( m_pclUI->sliderBrightness, SIGNAL(sliderReleased()), this, SLOT(setLightBrightness()) );
    connect( pcl_light.get(), SIGNAL(stateChanged()), this, SLOT(updateState()) );
}

void LightBulbWidget::addControl(const QString &strName, QLayout *pclLayout)
{
    QFormLayout* pcl_form = dynamic_cast<QFormLayout*>(m_pclUI->groupBox->layout());
    pcl_form->addRow( strName, pclLayout );
}

void LightBulbWidget::addControl(const QString &strName, QWidget *pclWidget)
{
    QFormLayout* pcl_form = dynamic_cast<QFormLayout*>(m_pclUI->groupBox->layout());
    pcl_form->addRow( strName, pclWidget );
}

void LightBulbWidget::update()
{
    auto pcl_light = getLight();
    m_pclUI->groupBox->setTitle("Light \""+pcl_light->id()+"\"");
    m_pclUI->labelName->setText(         pcl_light->name() );
    m_pclUI->labelManufacturer->setText( pcl_light->manufacturer() );
    m_pclUI->labelModel->setText(        pcl_light->modelID() );
    m_pclUI->labelFirmware->setText(     pcl_light->SWVersion() );

    updateState();
}

void LightBulbWidget::updateState()
{
    auto pcl_light = getLight();
    m_pclUI->checkReachable->setChecked( pcl_light->isReachable() );
    m_pclUI->checkOn->setChecked(        pcl_light->isOn() );
    m_pclUI->sliderBrightness->setValue( pcl_light->brightness() );
}

void LightBulbWidget::setLightBrightness()
{
    auto pcl_light = getLight();
    if ( pcl_light )
        pcl_light->setBrightness( static_cast<uint8_t>(m_pclUI->sliderBrightness->value()) );
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
