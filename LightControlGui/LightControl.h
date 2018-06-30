#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include <memory>
#include <QMainWindow>

namespace Ui {
class LightControl;
}

class NodeWidget;
class NetworkService;

class LightControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit LightControl(QWidget *parent = 0);
    ~LightControl() override;

public slots:
    void updateWidgets();

protected slots:
    void removeWidget(QString);

private:
    template<class NodeFactory,class WidgetFactory>
    QStringList updateWidget(QLayout* pclLayout);

    std::unique_ptr<Ui::LightControl> m_pclUI;
    std::map<QString,NodeWidget*> m_mapNodeWidgets;
};

#endif // LIGHTCONTROL_H
