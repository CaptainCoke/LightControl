#ifndef REMOTECONTROLWIDGET_H
#define REMOTECONTROLWIDGET_H

#include "SensorWidget.h"

class QTimer;

class RemoteControlWidget : public SensorWidget
{
    Q_OBJECT
public:
    ~RemoteControlWidget() override = default;

protected slots:
    void updateLastUpdated();

protected:
    using SensorWidget::SensorWidget;
    void createGui() override;
    void updateState() override;

    QLabel *m_pclLastUpdatedLabel, *m_pclButtonLabel;
    QTimer *m_pclLastUpdatedTimer;
};

#endif // REMOTECONTROLWIDGET_H
