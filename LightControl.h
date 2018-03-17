#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include <memory>
#include <QMainWindow>

namespace Ui {
class LightControl;
}

class LightBulbWidget;

class LightControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit LightControl(QWidget *parent = 0);
    ~LightControl();

public slots:
    void updateLights();

private:
    void updateLightWidgets( const QJsonObject& mapLights );

    std::unique_ptr<Ui::LightControl> m_pclUI;
    std::map<QString,LightBulbWidget*> m_mapLightWidgets;
};

#endif // LIGHTCONTROL_H
