#ifndef LIGHTGROUPSCENE_H
#define LIGHTGROUPSCENE_H

#include <memory>
#include <QString>
#include <QObject>

class LightBulbState;

class LightGroupScene : public QObject
{
    Q_OBJECT
public:
    LightGroupScene( const QString &strGroupId, const QString& strId );
    ~LightGroupScene() override;

    bool updateName( const QString& strName );
    const QString& id() const { return m_strId; }
    const QString& name() const { return m_strName; }

    const std::map<QString,LightBulbState>& getStates();

    void refreshSettings();

    bool isActive();

public slots:
    void apply();

signals:
    void settingsRefreshed();
    void sceneApplied();

protected:
    void setSceneData(const QJsonObject &rclObject);

private:
    QString m_strGroupId, m_strId, m_strName;
    std::map<QString,LightBulbState> m_mapLightStates;
};

#endif // LIGHTGROUPSCENE_H
