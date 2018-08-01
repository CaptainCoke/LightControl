#ifndef LIGHTBULBSTATE_H
#define LIGHTBULBSTATE_H

#include <QString>
#include <optional>
#include <memory>
#include "LightColor.h"
#include "LightTemperature.h"

class QJsonObject;
class LightBulb;

class LightBulbState
{
public:
    LightBulbState();
    ~LightBulbState();

    LightBulbState(const LightBulbState&);
    LightBulbState(LightBulbState&&);

    LightBulbState& operator=(const LightBulbState&);
    LightBulbState& operator=(LightBulbState&&);

    bool hasOn() const;
    bool hasBrightness() const;
    bool hasColor() const;
    bool hasTemperature() const;
    bool on() const;
    uint8_t brightness() const;
    LightColor color() const;
    LightTemperature temperature() const;

    LightBulbState& setOn( bool bOn );
    LightBulbState& setBrightness( uint8_t uiBrightness );
    LightBulbState& setColor( LightColor clColor );
    LightBulbState& setTemperature( LightTemperature clTemperature );

    bool operator==( const LightBulbState& rclOther ) const;
    bool operator!=( const LightBulbState& rclOther ) const;

    LightBulbState operator-( const LightBulbState& rclOther ) const;

    void updateSettingsFromJson( const QJsonObject& rclSettings );

    static LightBulbState fromSceneSettings( const QJsonObject& rclSettings );
    QJsonObject toJson( bool bForScene ) const;

    QStringList getStateAsText() const;

protected:
    std::optional<bool>             m_bOn;
    std::optional<uint8_t>          m_uiBrightness;
    std::optional<LightColor>       m_clColor;
    std::optional<LightTemperature> m_clTemperature;
};

QDebug& operator<<(QDebug& rclStream, const LightBulbState& rclState);

#endif // LIGHTBULBSTATE_H
