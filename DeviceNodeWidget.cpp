#include "DeviceNodeWidget.h"
#include <QMessageBox>
#include "ui_DeviceNodeWidget.h"
#include "DeviceNode.h"

DeviceNodeWidget::DeviceNodeWidget(QWidget *parent)
: QWidget(parent)
, m_pclUI(std::make_unique<Ui::DeviceNodeWidget>())
{
}

DeviceNodeWidget::~DeviceNodeWidget() = default;

void DeviceNodeWidget::createGui()
{
    m_pclUI->setupUi(this);
    auto pcl_node = getNode();
    connect( m_pclUI->buttonRefresh, &QPushButton::clicked, pcl_node.get(), &DeviceNode::refreshNode );
    connect( m_pclUI->buttonDelete, &QPushButton::clicked, this, &DeviceNodeWidget::deleteNode );
}


void DeviceNodeWidget::addControl(const QString &strName, QLayout *pclLayout)
{
    QFormLayout* pcl_form = dynamic_cast<QFormLayout*>(m_pclUI->groupBox->layout());
    pcl_form->addRow( strName, pclLayout );
}

void DeviceNodeWidget::addControl(const QString &strName, QWidget *pclWidget)
{
    QFormLayout* pcl_form = dynamic_cast<QFormLayout*>(m_pclUI->groupBox->layout());
    pcl_form->addRow( strName, pclWidget );
}

void DeviceNodeWidget::setNode(const std::shared_ptr<DeviceNode> &pclNode)
{
    m_pclNode = pclNode;
    connect( pclNode.get(), &DeviceNode::stateChanged, this, &DeviceNodeWidget::updateState );
}

void DeviceNodeWidget::updateNode()
{
    auto pcl_node = getNode();
    m_pclUI->groupBox->setTitle(pcl_node->nodeType()+" \""+pcl_node->id()+"\"");
    m_pclUI->labelName->setText(         pcl_node->name() );
    m_pclUI->labelManufacturer->setText( pcl_node->manufacturer() );
    m_pclUI->labelModel->setText(        pcl_node->modelID() );
    m_pclUI->labelFirmware->setText(     pcl_node->SWVersion() );
    updateState();
}

void DeviceNodeWidget::deleteNode()
{
    auto pcl_node = getNode();
    if ( QMessageBox::question( this, "Delete Node", QString("do you really want to delete %1 %2?").arg(pcl_node->nodeType()).arg(pcl_node->id()) ) != QMessageBox::Yes )
        return;
    pcl_node->deleteNode();
}

void DeviceNodeWidget::updateState()
{
    auto pcl_node = getNode();
    m_pclUI->checkReachable->setChecked( pcl_node->isReachable() );
}
