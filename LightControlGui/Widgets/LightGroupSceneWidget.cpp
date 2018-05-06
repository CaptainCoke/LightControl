#include "LightGroupSceneWidget.h"
#include <Nodes/LightGroupScene.h>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <LightBulbState.h>
#include <Nodes/LightBulb.h>
#include "LightTemperature.h"

LightGroupSceneWidget::LightGroupSceneWidget(QWidget *parent)
: QGroupBox(parent)
{
    setLayout(new QFormLayout);
}

LightGroupSceneWidget::~LightGroupSceneWidget() = default;

void LightGroupSceneWidget::setScene(std::shared_ptr<LightGroupScene> pclScene)
{
    setTitle( QString("Scene \"%1\": \"%2\"").arg( pclScene->id(), pclScene->name() ) );

    qDeleteAll(findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));

    QFormLayout* pcl_layout = dynamic_cast<QFormLayout*>(layout());

    for ( const auto &[str_id, rcl_state] : pclScene->getStates() )
    {
        QStringList lst_text;
        lst_text << (rcl_state.isOn() ? "on" : "off");
        if ( rcl_state.hasBrightness() )
            lst_text << QString("bri:%1").arg( rcl_state.brightness() );
        if ( rcl_state.hasColor() )
            lst_text << QString("col:%1,%2").arg( rcl_state.color().x() ).arg( rcl_state.color().y() );
        if ( rcl_state.hasTemperature() )
            lst_text << QString("temp:%1K").arg( rcl_state.temperature().kelvin() );

        pcl_layout->addRow( LightBulb::get(str_id)->name(), new QLabel(lst_text.join("\n")) );
    }
    QPushButton* pcl_apply_btn = new QPushButton("apply");
    connect( pcl_apply_btn, &QPushButton::clicked, pclScene.get(), &LightGroupScene::apply );
    pcl_layout->addRow("", pcl_apply_btn);

}
