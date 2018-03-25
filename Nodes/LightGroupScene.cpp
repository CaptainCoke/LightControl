#include "LightGroupScene.h"

LightGroup::Scene::Scene(const QString& strGroupId, const QString &strId, const QString &strName)
: m_strGroupId(strGroupId)
, m_strId(strId)
, m_strName(strName)
{
}

LightGroup::Scene::~Scene() = default;

void LightGroup::Scene::refreshSettings()
{
    //TODO
}

