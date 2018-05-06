#ifndef LIGHTBULBWIDGET_H
#define LIGHTBULBWIDGET_H

#include "DeviceNodeWidget.h"

class QSlider;
class QCheckBox;
class LightBulb;

class LightBulbWidget : public DeviceNodeWidget
{
    Q_OBJECT
public:
    ~LightBulbWidget() override;
    static LightBulbWidget* createWidget( const std::shared_ptr<LightBulb>& pclLight );

protected slots:
    void setLightBrightness();

protected:
    void createGui() override;
    void updateState() override;

private:
    explicit LightBulbWidget( const std::shared_ptr<LightBulb>& pclLight, QWidget *parent = nullptr );

    QCheckBox *m_pclOnCheck;
    QSlider   *m_pclBrightnessSlider;
};

#endif // LIGHTBULBWIDGET_H
