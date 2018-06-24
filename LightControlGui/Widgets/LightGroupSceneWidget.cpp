#include "LightGroupSceneWidget.h"
#include <Nodes/LightGroupScene.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <LightBulbState.h>
#include <Nodes/LightBulb.h>
#include "LightTemperature.h"

LightGroupSceneWidget::LightGroupSceneWidget(QWidget *parent)
: QGroupBox(parent)
{
    setLayout(new QGridLayout);
}

LightGroupSceneWidget::~LightGroupSceneWidget() = default;

void LightGroupSceneWidget::setScene(std::shared_ptr<LightGroupScene> pclScene)
{
    setTitle( QString("Scene \"%1\": \"%2\"").arg( pclScene->id(), pclScene->name() ) );

    qDeleteAll(findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));

    QGridLayout* pcl_layout = dynamic_cast<QGridLayout*>(layout());

    int i_row = 0;
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

        auto pcl_pick_button = new QPushButton("pick");
        connect( pcl_pick_button, &QPushButton::clicked, [this,str_id,pclScene]{ pclScene->pickSettings(str_id); } );

        pcl_layout->addWidget( new QLabel( LightBulb::get(str_id)->name() ), i_row, 0, 1, 1 );
        pcl_layout->addWidget( new QLabel(lst_text.join("\n")), i_row, 1, 2, 1 );
        pcl_layout->addWidget( pcl_pick_button, i_row+1, 0, 1, 1 );
        i_row+=2;
    }
    QPushButton* pcl_reset_btn = new QPushButton("reset");
    connect( pcl_reset_btn, &QPushButton::clicked, pclScene.get(), &LightGroupScene::refreshSettings );
    pcl_layout->addWidget( pcl_reset_btn, i_row, 0, 1, 1 );

    QPushButton* pcl_save_button = new QPushButton("save");
    connect( pcl_save_button, &QPushButton::clicked, pclScene.get(), &LightGroupScene::save );
    pcl_layout->addWidget( pcl_save_button, i_row, 1, 1, 1 );

    i_row++;

    QPushButton* pcl_apply_btn = new QPushButton("apply");
    connect( pcl_apply_btn, &QPushButton::clicked, pclScene.get(), &LightGroupScene::apply );
    pcl_layout->addWidget( pcl_apply_btn, i_row, 0, 1, 2 );
}
