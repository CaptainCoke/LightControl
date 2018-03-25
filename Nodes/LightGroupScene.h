#ifndef LIGHTGROUPSCENE_H
#define LIGHTGROUPSCENE_H

#include <memory>
#include <QString>

#include "LightGroup.h"

class LightGroup::Scene
{
public:
    Scene( const QString &strGroupId, const QString& strId, const QString& strName );
    ~Scene();

    const QString& id() const { return m_strId; }
    const QString& name() const { return m_strName; }

    void refreshSettings();

private:
    QString m_strGroupId, m_strId, m_strName;
};

#endif // LIGHTGROUPSCENE_H
