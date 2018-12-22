#include "LightGroupWidget.h"
#include <QMessageBox>
#include "ui_LightGroupWidget.h"
#include <Nodes/LightGroup.h>
#include <Nodes/LightBulb.h>
#include <Nodes/LightGroupScene.h>

LightGroupWidget::LightGroupWidget(const std::shared_ptr<LightGroup>& pclGroup, QWidget *parent)
: NodeWidget(parent)
, m_pclUI(std::make_unique<Ui::LightGroupWidget>())
{
    setNode( pclGroup );
    m_pclUI->setupUi(this);
    connect( m_pclUI->buttonRefresh, &QPushButton::clicked, pclGroup.get(), &Node::refreshNode );
    connect( m_pclUI->buttonDelete, &QPushButton::clicked, this, &NodeWidget::deleteNode );
    connect( m_pclUI->checkLightsOn, &QCheckBox::stateChanged, this, &LightGroupWidget::setLightOnState );
    connect( m_pclUI->listScenes, &QListWidget::currentItemChanged, this, &LightGroupWidget::showSceneInfo );
    connect( m_pclUI->buttonPreviousScene, &QPushButton::clicked, pclGroup.get(), &LightGroup::setPreviousScene );
    connect( m_pclUI->buttonNextScene, &QPushButton::clicked, pclGroup.get(), &LightGroup::setNextScene );
}

LightGroupWidget::~LightGroupWidget() = default;

void LightGroupWidget::updateNode()
{
    auto pcl_group = getNode<LightGroup>();
    m_pclUI->groupBox->setTitle(pcl_group->nodeType()+" \""+pcl_group->id()+"\"");
    m_pclUI->labelName->setText( pcl_group->name() );
    updateState();
}

void LightGroupWidget::deleteNode()
{
    auto pcl_group = getNode<LightGroup>();
    if ( QMessageBox::question( this, "Delete Group", QString("do you really want to delete group %1 (%2)?").arg(pcl_group->name()).arg(pcl_group->id()) ) != QMessageBox::Yes )
        return;
    pcl_group->deleteNode();
}

void LightGroupWidget::updateState()
{
    auto pcl_group = getNode<LightGroup>();
    QSignalBlocker cl_block(m_pclUI->checkLightsOn);
    m_pclUI->checkLightsOn->setCheckState( pcl_group->allOn() ? Qt::Checked : (pcl_group->anyOn() ? Qt::PartiallyChecked : Qt::Unchecked) );

    m_pclUI->listLights->clear();
    for ( const auto &pcl_light : pcl_group->lights() )
        if ( pcl_light )
            m_pclUI->listLights->addItem( pcl_light->name() );

    int i_selected_scene = m_pclUI->listScenes->currentRow();
    m_pclUI->listScenes->clear();
    m_pclUI->sceneWidget->hide();
    auto pcl_current_scene = pcl_group->getCurrentScene();
    QFont sel_font = m_pclUI->listScenes->font();
    sel_font.setBold(true);
    for ( const auto &[str_id,pcl_scene] : pcl_group->scenes() )
    {
        auto pcl_item = new QListWidgetItem( pcl_scene->name() );
        pcl_item->setData( Qt::UserRole, str_id );
        if ( pcl_scene == pcl_current_scene )
            pcl_item->setFont( sel_font );
        m_pclUI->listScenes->addItem( pcl_item );
    }
    m_pclUI->listScenes->setCurrentRow(i_selected_scene);
}

void LightGroupWidget::setLightOnState(int iState)
{
    auto pcl_group = getNode<LightGroup>();
    bool b_all_lights_on;
    switch (iState) {
    case Qt::Checked: b_all_lights_on = true; break;
    case Qt::Unchecked: b_all_lights_on = false; break;
    case Qt::PartiallyChecked:
    default: //there shouldn't be any other cases
        b_all_lights_on = pcl_group->anyOn() ? false : true;
        break;
    }

    auto pcl_current_scene = pcl_group->getCurrentScene();
    if ( pcl_current_scene && b_all_lights_on )
        pcl_current_scene->apply();
    else
        for ( const auto &pcl_light : pcl_group->lights() )
            pcl_light->setOn(b_all_lights_on);
}

void LightGroupWidget::showSceneInfo(QListWidgetItem* pclItem, QListWidgetItem*)
{
    m_pclUI->sceneWidget->hide();
    if ( pclItem )
    {
        auto pcl_group = getNode<LightGroup>();
        auto it_scene = pcl_group->scenes().find(pclItem->data(Qt::UserRole).toString());
        if ( it_scene != pcl_group->scenes().end() )
        {
            m_pclUI->sceneWidget->setScene(it_scene->second);
            m_pclUI->sceneWidget->show();
        }
    }
}

LightGroupWidget* LightGroupWidget::createWidget(const std::shared_ptr<LightGroup>& pclGroup)
{
    LightGroupWidget* pcl_widget = new LightGroupWidget(pclGroup);
    pcl_widget->updateNode();
    return pcl_widget;
}

