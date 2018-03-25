#ifndef LIGHTGROUPWIDGET_H
#define LIGHTGROUPWIDGET_H

#include "NodeWidget.h"

namespace Ui {
class LightGroupWidget;
}

class LightGroup;

class LightGroupWidget : public NodeWidget
{
    Q_OBJECT
public:
    ~LightGroupWidget() override;
    static LightGroupWidget* createWidget( const std::shared_ptr<LightGroup>& pclGroup );

    void updateNode() override;

public slots:
    void deleteNode() override;
    void updateState() override;

protected slots:
    void setLightOnState( int iState );

private:
    explicit LightGroupWidget(const std::shared_ptr<LightGroup>& pclGroup, QWidget *parent = nullptr);

    std::unique_ptr<Ui::LightGroupWidget> m_pclUI;

};

#endif // LIGHTGROUPWIDGET_H
