#ifndef CTLIGHTBULBWIDGET_H
#define CTLIGHTBULBWIDGET_H

#include "LightBulbWidget.h"

class QSlider;
class QSpinBox;
class QLabel;

class CTLightBulbWidget : public LightBulbWidget
{
    Q_OBJECT
public:
    ~CTLightBulbWidget() override = default;

protected slots:
    void setLightTemperature();
    void updateValueFromInput(int iMired);

protected:
    using LightBulbWidget::LightBulbWidget;

    void createGui() override;
    void updateState() override;

    QSlider *m_pclTemperatureSlider;
    QSpinBox* m_pclTemperatureSpinBox;
    QLabel* m_pclTemperatureLabel;
};

#endif // CTLIGHTBULBWIDGET_H
