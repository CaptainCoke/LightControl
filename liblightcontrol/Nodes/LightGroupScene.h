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
    LightGroupScene( const QString &strGroupId, const QString& strId, const QString& strName );
    ~LightGroupScene() override;

    const QString& id() const { return m_strId; }
    const QString& name() const { return m_strName; }

    const std::map<QString,LightBulbState>& getStates();

    void refreshSettings();

public slots:
    void apply();

signals:
    void settingsRefreshed();

protected:
    void setSceneData(const QJsonObject &rclObject);

private:
    QString m_strGroupId, m_strId, m_strName;
    std::map<QString,LightBulbState> m_mapLightStates;
};

#endif // LIGHTGROUPSCENE_H
