#ifndef LIGHTBULB_H
#define LIGHTBULB_H

#include <memory>
#include "DeviceNode.h"

class LightBulb : public DeviceNode
{
    Q_OBJECT
public:
    ~LightBulb() override;

    bool isOn() const { return m_bOn; }
    uint8_t brightness() const { return m_uiBrightness; }

    void setNodeData(const QJsonObject &rclObject) override;

    static std::shared_ptr<LightBulb> create(const QString& strId, const QJsonObject &rclObject);
    static std::shared_ptr<LightBulb> get(const QString& strId);

    QString nodeType() const override;

public slots:
    void setOn( bool bOn );
    void setBrightness( uint8_t uiBrightness, float fTransitionTimeS = 0.f );

protected:
    using DeviceNode::DeviceNode;
    virtual bool setStateData(const QJsonObject &rclObject);

private:
    bool    m_bOn;
    uint8_t m_uiBrightness;

    static std::map<QString,std::shared_ptr<LightBulb>> s_mapLights;
};

#endif // LIGHTBULB_H
