#ifndef LIGHTBULBWIDGET_H
#define LIGHTBULBWIDGET_H

#include "DeviceNodeWidget.h"

class QSlider;
class QCheckBox;
class LightBulb;
class QLabel;

class LightBulbWidget : public DeviceNodeWidget
{
    Q_OBJECT
public:
    ~LightBulbWidget() override;
    static LightBulbWidget* createWidget( const std::shared_ptr<LightBulb>& pclLight );

protected slots:
    void setLightBrightness();
    void setOn(bool bOn);
    void updateLastPoweredText();

protected:
    void createGui() override;
    void updateState() override;
    void setToTargetStateReached();
    void setToTargetStateLost();
    void setToTargetStateChanging();

    void setLastPoweredText( const QDateTime& tLastPowered );

private:
    explicit LightBulbWidget( const std::shared_ptr<LightBulb>& pclLight, QWidget *parent = nullptr );

    QCheckBox *m_pclOnCheck;
    QSlider   *m_pclBrightnessSlider;
    QLabel    *m_pclStateReachedLabel;
    QLabel    *m_pclLastPoweredLabel;
};

#endif // LIGHTBULBWIDGET_H
