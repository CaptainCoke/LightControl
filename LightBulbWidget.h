#ifndef LIGHTBULBWIDGET_H
#define LIGHTBULBWIDGET_H

#include <memory>
#include <QWidget>

namespace Ui {
class LightBulbWidget;
}

class LightBulb;

class LightBulbWidget : public QWidget
{
    Q_OBJECT

public:
    ~LightBulbWidget() override;
    static LightBulbWidget* createWidget( const std::shared_ptr<LightBulb>& pclLight );

public slots:
    void update();
    virtual void updateState();

protected slots:
    void setLightBrightness();

protected:
    virtual void createGui();
    void addControl( const QString& strName, QLayout* pclLayout );
    void addControl( const QString& strName, QWidget* pclWidget );

    template<class LightType = LightBulb>
    std::shared_ptr<LightType> getLight() { return std::dynamic_pointer_cast<LightType>(m_pclLight.lock()); }
private:
    explicit LightBulbWidget( const std::shared_ptr<LightBulb>& pclLight, QWidget *parent = nullptr );
    std::unique_ptr<Ui::LightBulbWidget> m_pclUI;
    std::weak_ptr<LightBulb> m_pclLight;
};

#endif // LIGHTBULBWIDGET_H
