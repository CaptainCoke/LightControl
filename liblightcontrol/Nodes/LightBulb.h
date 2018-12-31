#ifndef LIGHTBULB_H
#define LIGHTBULB_H

#include "DeviceNode.h"
#include "NodeFactory.h"
#include <functional>
#include <QDateTime>

class LightBulbState;

class LightBulb : public DeviceNode, public NodeFactory<LightBulb>
{
    Q_OBJECT
public:
    ~LightBulb() override;
    static const QString node_type;

    bool isOn() const;

    void setNodeData(const QJsonObject &rclObject) override;
    virtual void handlePushUpdate(const QJsonObject &rclObject);

    const QString& nodeType() const override;

    const LightBulbState& getCurrentState() const;
    const LightBulbState& getTargetState() const;
    bool isInTargetState() const;

signals:
    void targetStateReached();
    void targetStateLost();
    void targetStateChanged();

public slots:
    void setOn( bool bOn, float fTransitionTimeS = 0.f );
    void setTargetState( const LightBulbState& rclState, const QDateTime& rclWhen );
    void setTargetState( const LightBulbState& rclState, float fSecondsInTheFuture );
    void changeToTargetStateIfNecessary();

protected slots:
    void onFalseState();

protected:
    LightBulb();

    void initializeNode(const QString& strId) override;

    void putStateOnLightBulb( const LightBulbState& rclState, float fTransitionTimeSeconds = 0.f);
    bool setStateData(const QJsonObject &rclObject);

    friend class NodeFactory<LightBulb>;
    static std::shared_ptr<LightBulb> createNode(const QString& strId, const QJsonObject &rclObject);

    LightBulbState& getCurrentState();

    void reactOnTargetState();
    virtual bool isCloseEnoughToTargetState() const;

private:
    std::unique_ptr<LightBulbState> m_pclCurrentState, m_pclTargetState;
    QDateTime m_clTargetStateTimepoint; //< the timepoint when on the target state is supposed to be reached
    bool m_bIsInTargetState = false;
    QTimer m_clStateLostGracePeriod;
};

#endif // LIGHTBULB_H
