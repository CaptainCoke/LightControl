#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <memory>

template<typename NodeType>
class NodeFactory {
public:
    static std::shared_ptr<NodeType> create(const QString& strId, const QJsonObject &rclObject)
    {
        auto pcl_node = NodeType::createNode( strId, rclObject );
        pcl_node->setNodeData( rclObject );
        allNodes()[strId] = pcl_node;
        return pcl_node;
    }

    static std::shared_ptr<NodeType> get(const QString& strId)
    {
        auto it_light = allNodes().find(strId);
        if ( it_light != allNodes().end() )
            return it_light->second;
        else
            return nullptr;
    }

    static void remove(const QString& strId)
    {
        auto it_light = allNodes().find(strId);
        if ( it_light != allNodes().end() )
            allNodes().erase( it_light );
    }

    static void clearAllNodes()
    {
        allNodes().clear();
    }

    static const std::map<QString,std::shared_ptr<NodeType>>& getAll()
    {
        return allNodes();
    }
private:
    static std::map<QString,std::shared_ptr<NodeType>>& allNodes()
    {
        static std::map<QString,std::shared_ptr<NodeType>> s_map_nodes;
        return s_map_nodes;
    }
};

#endif // NODEFACTORY_H
