#ifndef CTLIGHTBULBWIDGET_H
#define CTLIGHTBULBWIDGET_H

#include "LightBulbWidget.h"

class CTLightBulbWidget : public LightBulbWidget
{
public:
    ~CTLightBulbWidget() override = default;
protected:
    using LightBulbWidget::LightBulbWidget;
};

#endif // CTLIGHTBULBWIDGET_H
