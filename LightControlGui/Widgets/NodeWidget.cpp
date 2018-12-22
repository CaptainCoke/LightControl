#include "NodeWidget.h"
#include <QMessageBox>
#include <Nodes/Node.h>

NodeWidget::NodeWidget(QWidget *parent)
: QWidget(parent)
{
}

NodeWidget::~NodeWidget() = default;

void NodeWidget::setNode(const std::shared_ptr<Node> &pclNode)
{
    m_pclNode = pclNode;
    connect( pclNode.get(), &Node::stateChanged, this, &NodeWidget::updateState );
}

void NodeWidget::deleteNode()
{
    auto pcl_node = getNode();
    if ( QMessageBox::question( this, "Delete Node", QString("do you really want to delete %1 %2?").arg(pcl_node->nodeType()).arg(pcl_node->id()) ) != QMessageBox::Yes )
        return;
    pcl_node->deleteNode();
}
