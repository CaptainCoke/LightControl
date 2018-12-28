#include "LightGroupSceneWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <LightBulbState.h>
#include <Nodes/LightGroupScene.h>
#include <Nodes/LightBulb.h>
#include <LightTemperature.h>

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

    QPushButton* pcl_store_btn = new QPushButton("group state -> scene");
    connect( pcl_store_btn, &QPushButton::clicked, pclScene.get(), &LightGroupScene::storeCurrentGroupState );
    pcl_layout->addWidget( pcl_store_btn, i_row, 0, 1, 2 );
    i_row++;

    for ( const auto &[str_id, rcl_state] : pclScene->getStates() )
    {
        auto pcl_pick_button = new QPushButton("pick");
        connect( pcl_pick_button, &QPushButton::clicked, [this,str_id,pclScene]{ pclScene->pickSettings(str_id); } );

        pcl_layout->addWidget( new QLabel( LightBulb::get(str_id)->name() ), i_row, 0, 1, 1 );
        pcl_layout->addWidget( new QLabel(rcl_state.getStateAsText().join("\n")), i_row, 1, 2, 1 );
        pcl_layout->addWidget( pcl_pick_button, i_row+1, 0, 1, 1 );
        i_row+=2;
    }
    QDoubleSpinBox* pcl_spin_box = new QDoubleSpinBox();
    pcl_spin_box->setMinimum( 0 );
    pcl_spin_box->setMaximum( std::numeric_limits<double>::max() );
    pcl_spin_box->setValue( pclScene->getTransitionTime() );
    pcl_spin_box->setSuffix("s");
    connect(pcl_spin_box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), pclScene.get(), &LightGroupScene::setTransitionTime );
    pcl_layout->addWidget( new QLabel("transition"), i_row, 0, 1, 1 );
    pcl_layout->addWidget( pcl_spin_box, i_row, 1, 1, 1 );
    i_row++;

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
