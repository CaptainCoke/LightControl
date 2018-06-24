#ifndef LIGHTBULB_H
#define LIGHTBULB_H

#include "DeviceNode.h"
#include "NodeFactory.h"

class LightBulbState;

class LightBulb : public DeviceNode, public NodeFactory<LightBulb>
{
    Q_OBJECT
public:
    ~LightBulb() override;

    bool isOn() const { return m_bOn; }
    uint8_t brightness() const { return m_uiBrightness; }

    void setNodeData(const QJsonObject &rclObject) override;

    QString nodeType() const override;

    virtual void setToState( const LightBulbState& rclState );
    virtual LightBulbState getCurrentState() const;

public slots:
    void setOn( bool bOn );
    void setBrightness( uint8_t uiBrightness, float fTransitionTimeS = 0.f );

protected:
    using DeviceNode::DeviceNode;
    virtual bool setStateData(const QJsonObject &rclObject);

    friend class NodeFactory<LightBulb>;
    static std::shared_ptr<LightBulb> createNode(const QString& strId, const QJsonObject &rclObject);

private:
    bool    m_bOn;
    uint8_t m_uiBrightness;
};

#endif // LIGHTBULB_H
