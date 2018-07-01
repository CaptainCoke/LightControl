#ifndef SENSOR_H
#define SENSOR_H

#include "DeviceNode.h"
#include "NodeFactory.h"

class Sensor : public DeviceNode, public NodeFactory<Sensor>
{
public:
    static const QString node_type;

    ~Sensor() override;

    bool isOn() const { return m_bOn; }
    uint8_t battery() const { return m_uiBattery; }

    void setNodeData(const QJsonObject &rclObject) override;
    virtual void handlePushUpdate(const QJsonObject &rclObject);

    const QString& nodeType() const override;

protected:
    using DeviceNode::DeviceNode;
    virtual bool setConfigData(const QJsonObject &rclObject);
    virtual bool setStateData(const QJsonObject &rclObject);

    friend class NodeFactory<Sensor>;
    static std::shared_ptr<Sensor> createNode(const QString& strId, const QJsonObject &rclObject);

private:
    bool    m_bOn;
    uint8_t m_uiBattery;
};

#endif // SENSOR_H
