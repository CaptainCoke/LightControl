#ifndef SENSORWIDGET_H
#define SENSORWIDGET_H

#include "DeviceNodeWidget.h"

class QProgressBar;
class QLabel;
class Sensor;
class QCheckBox;

class SensorWidget : public DeviceNodeWidget
{
    Q_OBJECT
public:
    ~SensorWidget() override;
    static SensorWidget* createWidget( const std::shared_ptr<Sensor>& pclSensor );

protected:
    void createGui() override;
    void updateState() override;

private:
    explicit SensorWidget( const std::shared_ptr<Sensor>& pclSensor, QWidget *parent = nullptr);

    QCheckBox    *m_pclOnCheck;
    QProgressBar *m_pclBatteryBar;
};

#endif // SENSORWIDGET_H
