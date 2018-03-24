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

    void updateLightsFromGroup( const QString& strGroupId );

    QLabel *m_pclLastUpdatedLabel, *m_pclButtonLabel, *m_pclGroupLabel;
    QTimer *m_pclLastUpdatedTimer;
};

#endif // REMOTECONTROLWIDGET_H
