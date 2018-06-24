#ifndef LIGHTGROUP_H
#define LIGHTGROUP_H

#include "Node.h"
#include "NodeFactory.h"

class LightBulb;
class LightGroupScene;

class LightGroup : public Node, public NodeFactory<LightGroup>
{
public:
    ~LightGroup() override;

    bool allOn() const { return m_bAllOn; }
    bool anyOn() const { return m_bAnyOn; }
    std::list<std::shared_ptr<LightBulb>> lights() const;
    const std::map<QString,std::shared_ptr<LightGroupScene>>& scenes() const { return m_mapScenes; }

    void setNodeData(const QJsonObject &rclObject) override;

    QString nodeType() const override;

    std::shared_ptr<LightGroupScene> getCurrentScene() const;

public slots:
    void setNextScene();
    void setPreviousScene();

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
