#ifndef DEVICENODEWIDGET_H
#define DEVICENODEWIDGET_H

#include "NodeWidget.h"

namespace Ui {
class DeviceNodeWidget;
}

class DeviceNodeWidget : public NodeWidget
{
public:
    explicit DeviceNodeWidget(QWidget *parent = nullptr);
    ~DeviceNodeWidget() override;

public slots:
    void updateNode();
    void updateState() override;

protected:
    virtual void createGui();
    void addControl( const QString& strName, QLayout* pclLayout );
    void addControl( const QString& strName, QWidget* pclWidget );

private:
    std::unique_ptr<Ui::DeviceNodeWidget> m_pclUI;
};

#endif // DEVICENODEWIDGET_H
