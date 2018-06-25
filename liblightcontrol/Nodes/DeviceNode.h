#ifndef DEVICENODE_H
#define DEVICENODE_H

#include "Node.h"

class DeviceNode : public Node
{
    Q_OBJECT
public:
    ~DeviceNode() override;

    const QString& manufacturer() const { return m_strManufacturer; }
    const QString& modelID() const { return m_strModelID; }
    const QString& SWVersion() const { return m_strSWVersion; }

    void setNodeData(const QJsonObject &rclObject) override;
    bool isReachable() const { return m_bReachable; }

signals:
    void changeStateConfirmed();

protected:
    using Node::Node;

    bool setReachable( bool bReachable );
    void changeState( QJsonObject clObject, float fTransitionTimeS = 0.f );

private:
    QString m_strManufacturer;
    QString m_strModelID;
    QString m_strSWVersion;
    bool    m_bReachable;
};

#endif // DEVICENODE_H
