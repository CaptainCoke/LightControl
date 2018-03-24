#ifndef DEVICENODEWIDGET_H
#define DEVICENODEWIDGET_H

#include <memory>
#include <QWidget>

namespace Ui {
class DeviceNodeWidget;
}

class DeviceNode;

class DeviceNodeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceNodeWidget(QWidget *parent = nullptr);
    ~DeviceNodeWidget() override;

public slots:
    void updateNode();
    void deleteNode();
    virtual void updateState();

protected:
    virtual void createGui();
    void addControl( const QString& strName, QLayout* pclLayout );
    void addControl( const QString& strName, QWidget* pclWidget );

    template<class NodeType = DeviceNode>
    std::shared_ptr<NodeType> getNode() { return std::dynamic_pointer_cast<NodeType>(m_pclNode.lock()); }

    void setNode( const std::shared_ptr<DeviceNode>& pclNode );

private:
    std::unique_ptr<Ui::DeviceNodeWidget> m_pclUI;

    std::weak_ptr<DeviceNode> m_pclNode;
};

#endif // DEVICENODEWIDGET_H
