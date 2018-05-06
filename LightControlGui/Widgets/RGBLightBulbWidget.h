#ifndef RGBLIGHTBULBWIDGET_H
#define RGBLIGHTBULBWIDGET_H

#include "LightBulbWidget.h"

class QSlider;

class RGBLightBulbWidget : public LightBulbWidget
{
    Q_OBJECT
public:
    ~RGBLightBulbWidget() override = default;

protected slots:
    void setLightHue();
    void setLightSaturation();
    void setLightTemperature();

protected:
    using LightBulbWidget::LightBulbWidget;
    void createGui() override;
    void updateState() override;

    QSlider *m_pclHueSlider, *m_pclSaturationSlider;
    QSlider *m_pclTemperatureSlider;
};

#endif // RGBLIGHTBULBWIDGET_H
