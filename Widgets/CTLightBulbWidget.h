#ifndef CTLIGHTBULBWIDGET_H
#define CTLIGHTBULBWIDGET_H

#include "LightBulbWidget.h"

class QSlider;

class CTLightBulbWidget : public LightBulbWidget
{
    Q_OBJECT
public:
    ~CTLightBulbWidget() override = default;

protected slots:
    void setLightTemperature();

protected:
    using LightBulbWidget::LightBulbWidget;

    void createGui() override;
    void updateState() override;

    QSlider *m_pclTemperatureSlider;
};

#endif // CTLIGHTBULBWIDGET_H
