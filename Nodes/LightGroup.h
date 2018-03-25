#ifndef LIGHTGROUP_H
#define LIGHTGROUP_H

#include "Node.h"
#include <memory>

class LightBulb;

class LightGroup : public Node
{
public:
    class Scene;

    ~LightGroup() override;

    bool allOn() const { return m_bAllOn; }
    bool anyOn() const { return m_bAnyOn; }
    std::list<std::shared_ptr<LightBulb>> lights() const;
    const std::list<std::shared_ptr<Scene>>& scenes() const { return m_lstScenes; }

    void setNodeData(const QJsonObject &rclObject) override;

    static std::shared_ptr<LightGroup> create(const QString& strId, const QJsonObject &rclObject);
    static std::shared_ptr<LightGroup> get(const QString& strId);
    static void remove(const QString& strId);

    QString nodeType() const override;

protected:
    using Node::Node;
    bool setStateData(const QJsonObject &rclObject);
    bool setLights(const QJsonArray& rclArray);
    bool setScenes(const QJsonArray& rclArray);

private:
    QStringList m_lstLightIds;
    bool        m_bAllOn;
    bool        m_bAnyOn;

    std::list<std::shared_ptr<Scene>> m_lstScenes;

    static std::map<QString,std::shared_ptr<LightGroup>> s_mapGroups;
};
#endif // LIGHTGROUP_H
