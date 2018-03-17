#ifndef LIGHTBULB_H
#define LIGHTBULB_H

#include <memory>
#include <QString>
#include <QObject>

class LightBulb : public QObject
{
    Q_OBJECT
public:
    ~LightBulb() override;

    const QString& id() const { return m_strId; }
    const QString& name() const { return m_strName; }
    const QString& manufacturer() const { return m_strManufacturer; }
    const QString& modelID() const { return m_strModelID; }
    const QString& SWVersion() const { return m_strSWVersion; }


    bool isReachable() const { return m_bReachable; }
    bool isOn() const { return m_bOn; }
    uint8_t brightness() const { return m_uiBrightness; }

    void setLightData(const QJsonObject &rclObject);

    static std::shared_ptr<LightBulb> create(const QString& strId, const QJsonObject &rclObject);
    static std::shared_ptr<LightBulb> get(const QString& strId);

public slots:
    void setOn( bool bOn );
    void setBrightness( uint8_t uiBrightness, float fTransitionTimeS = 0.f );
    void refreshState();

signals:
    void stateChanged();

protected:
    LightBulb(const QString& strId);
    virtual bool setStateData(const QJsonObject &rclObject);
    void changeState( QJsonObject clObject, float fTransitionTimeS = 0.f );

    QString m_strId;
    QString m_strName;
    QString m_strManufacturer;
    QString m_strModelID;
    QString m_strSWVersion;

    bool    m_bReachable;
    bool    m_bOn;
    uint8_t m_uiBrightness;

    static std::map<QString,std::shared_ptr<LightBulb>> s_mapLights;
};

#endif // LIGHTBULB_H
