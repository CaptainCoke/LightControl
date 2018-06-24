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
    LightBulbState( bool bOn );
    ~LightBulbState();

    LightBulbState(const LightBulbState&);
    LightBulbState(LightBulbState&&);

    LightBulbState& operator=(const LightBulbState&);
    LightBulbState& operator=(LightBulbState&&);

    bool isOn() const;
    bool hasBrightness() const;
    bool hasColor() const;
    bool hasTemperature() const;
    uint8_t brightness() const;
    LightColor color() const;
    LightTemperature temperature() const;

    void setOn( bool bOn );
    void setBrightness( uint8_t uiBrightness );
    void setColor( LightColor clColor );
    void setTemperature( LightTemperature clTemperature );

    bool operator==( const LightBulbState& rclOther );
    bool operator!=( const LightBulbState& rclOther );

    static LightBulbState fromSceneSettings( const QJsonObject& rclSettings );
    QJsonObject toJson() const;

protected:
    bool                            m_bOn;
    std::optional<uint8_t>          m_uiBrightness;
    std::optional<LightColor>       m_clColor;
    std::optional<LightTemperature> m_clTemperature;
};

#endif // LIGHTBULBSTATE_H
