#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include <QWidget>
#include <memory>

class Node;

class NodeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NodeWidget(QWidget *parent = nullptr);
    ~NodeWidget() override;

    virtual void updateNode() = 0;

public slots:
    virtual void deleteNode();
    virtual void updateState() = 0;

protected:
    template<class NodeType = Node>
    std::shared_ptr<NodeType> getNode() { return std::dynamic_pointer_cast<NodeType>(m_pclNode.lock()); }

    void setNode( const std::shared_ptr<Node>& pclNode );

private:
    std::weak_ptr<Node> m_pclNode;
};

#endif // NODEWIDGET_H
