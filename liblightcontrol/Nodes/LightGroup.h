#ifndef LIGHTGROUP_H
#define LIGHTGROUP_H

#include "Node.h"
#include "NodeFactory.h"

class LightBulb;
class LightGroupScene;

class LightGroup : public Node, public NodeFactory<LightGroup>
{
    Q_OBJECT
public:
    ~LightGroup() override;
    static const QString node_type;

    bool allOn() const;
    bool anyOn() const;
    std::list<std::shared_ptr<LightBulb>> lights() const;
    const std::map<QString,std::shared_ptr<LightGroupScene>>& scenes() const { return m_mapScenes; }

    void setNodeData(const QJsonObject &rclObject) override;
    virtual void handlePushUpdate(const QJsonObject &rclObject);

    const QString& nodeType() const override;
    bool isEmpty() const override;

    std::shared_ptr<LightGroupScene> getCurrentScene() const;
    std::shared_ptr<LightGroupScene> getNextScene() const;
    std::shared_ptr<LightGroupScene> getPreviousScene() const;

signals:
    void sceneCalled(const QString& strCalledSceneId); //< signals if a scene of this group was called

public slots:
    void setNextScene();
    void setPreviousScene();

protected slots:
    void lightChanged();

protected:
    using Node::Node;
    bool setStateData(const QJsonObject &rclObject);
    bool setLights(const QJsonArray& rclArray);
    bool setScenes(const QJsonArray& rclArray);

    void setCurrentScene( const QString& strSceneId );

    friend class NodeFactory<LightGroup>;
    static std::shared_ptr<LightGroup> createNode(const QString& strId, const QJsonObject &rclObject);

private:
    QStringList m_lstLightIds;
    bool        m_bAllOn;
    bool        m_bAnyOn;
    QString     m_strCurrentScene;

    std::map<QString,std::shared_ptr<LightGroupScene>> m_mapScenes;
};
#endif // LIGHTGROUP_H
