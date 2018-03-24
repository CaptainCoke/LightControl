#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include <memory>
#include <QMainWindow>

namespace Ui {
class LightControl;
}

class DeviceNodeWidget;

class LightControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit LightControl(QWidget *parent = 0);
    ~LightControl();

public slots:
    void updateFullState();

protected slots:
    void removeWidget(QString);

private:
    void updateFullState( const QJsonObject& mapState );
    void updateLightWidgets( const QJsonObject& mapLights );
    void updateSensorWidgets( const QJsonObject& mapSensors );
    void updateGroups( const QJsonObject& mapGroups );
    template<class NodeFactory,class WidgetFactory>
    void updateWidget(const QJsonObject& mapNodeData, QLayout* pclLayout);

    std::unique_ptr<Ui::LightControl> m_pclUI;
    std::map<QString,DeviceNodeWidget*> m_mapNodeWidgets;
};

#endif // LIGHTCONTROL_H
