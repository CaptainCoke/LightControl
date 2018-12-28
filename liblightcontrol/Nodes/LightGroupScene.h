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
    float getTransitionTime() const { return m_fTransitionTimeS; }

    const std::map<QString,LightBulbState>& getStates();

    bool isActive();

    void swapIdWith( LightGroupScene& rclOther );

public slots:
    void setTransitionTime( double fTransitionTimeS );
    void notifyLightsOfActivation();
    void apply();
    void storeCurrentGroupState();
    void save();
    void refreshSettings();
    void pickSettings( const QString& strLightId );

signals:
    void settingsRefreshed();
    void sceneApplied();

protected:
    void setSceneData(const QJsonObject &rclObject);

private:
    QString m_strGroupId, m_strId, m_strName;
    std::map<QString,LightBulbState> m_mapLightStates;
    float m_fTransitionTimeS = 0.f;
};

#endif // LIGHTGROUPSCENE_H
