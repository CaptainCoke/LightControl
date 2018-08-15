#include "DeviceNodeWidget.h"
#include "ui_DeviceNodeWidget.h"
#include "Nodes/DeviceNode.h"

DeviceNodeWidget::DeviceNodeWidget(QWidget *parent)
: NodeWidget(parent)
, m_pclUI(std::make_unique<Ui::DeviceNodeWidget>())
{
}

DeviceNodeWidget::~DeviceNodeWidget() = default;

void DeviceNodeWidget::createGui()
{
    m_pclUI->setupUi(this);
    auto pcl_node = getNode();
    connect( m_pclUI->buttonRefresh, &QPushButton::clicked, pcl_node.get(), &Node::refreshNode );
    connect( m_pclUI->buttonDelete, &QPushButton::clicked, this, &NodeWidget::deleteNode );
    connect( m_pclUI->groupBox, &QGroupBox::toggled, m_pclUI->content, &QWidget::setVisible );
}


void DeviceNodeWidget::addControl(const QString &strName, QLayout *pclLayout)
{
    QFormLayout* pcl_form = dynamic_cast<QFormLayout*>(m_pclUI->content->layout());
    pcl_form->addRow( strName, pclLayout );
}

void DeviceNodeWidget::addControl(const QString &strName, QWidget *pclWidget)
{
    QFormLayout* pcl_form = dynamic_cast<QFormLayout*>(m_pclUI->content->layout());
    pcl_form->addRow( strName, pclWidget );
}

void DeviceNodeWidget::updateNode()
{
    auto pcl_node = getNode<DeviceNode>();
    m_pclUI->groupBox->setTitle(QString("%1 \"%2\" (\"%3\")").arg(pcl_node->nodeType(),pcl_node->id(),pcl_node->name()));
    m_pclUI->labelName->setText(         pcl_node->name() );
    m_pclUI->labelManufacturer->setText( pcl_node->manufacturer() );
    m_pclUI->labelModel->setText(        pcl_node->modelID() );
    m_pclUI->labelFirmware->setText(     pcl_node->SWVersion() );
    updateState();
}



void DeviceNodeWidget::updateState()
{
    auto pcl_node = getNode<DeviceNode>();
    m_pclUI->checkReachable->setChecked( pcl_node->isReachable() );
}
