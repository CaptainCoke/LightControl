#ifndef LIGHTGROUPSCENEWIDGET_H
#define LIGHTGROUPSCENEWIDGET_H

#include <QGroupBox>
#include <memory>

class LightGroupScene;

class LightGroupSceneWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit LightGroupSceneWidget(QWidget *parent = nullptr);
    ~LightGroupSceneWidget() override;

    void setScene( std::shared_ptr<LightGroupScene> pclScene );
};

#endif // LIGHTGROUPSCENEWIDGET_H
