#ifndef SENSOR_H
#define SENSOR_H

#include <memory>
#include "DeviceNode.h"

class Sensor : public DeviceNode
{
public:
    ~Sensor() override;

    bool isOn() const { return m_bOn; }
    uint8_t battery() const { return m_uiBattery; }

    void setNodeData(const QJsonObject &rclObject) override;

    static std::shared_ptr<Sensor> create(const QString& strId, const QJsonObject &rclObject);
    static std::shared_ptr<Sensor> get(const QString& strId);
    static void remove(const QString& strId);

    QString nodeType() const override;

protected:
    using DeviceNode::DeviceNode;
    virtual bool setConfigData(const QJsonObject &rclObject);
    virtual bool setStateData(const QJsonObject &rclObject);

private:
    bool    m_bOn;
    uint8_t m_uiBattery;

    static std::map<QString,std::shared_ptr<Sensor>> s_mapSensors;
};

#endif // SENSOR_H
