#ifndef LIGHTBULBSTATE_H
#define LIGHTBULBSTATE_H

#include <QString>
#include <optional>
#include <memory>

#include "LightColor.h"

class QJsonObject;
class LightBulb;

class LightBulbState
{
public:
    LightBulbState( bool bOn );
    ~LightBulbState();

    bool isOn() const;
    bool hasBrightness() const;
    uint8_t brightness() const;

    void setOn( bool bOn );
    void setBrightness( uint8_t uiBrightness );
    void setColor( LightColor clColor );
    void setTemperature( int iTemperature );

    static LightBulbState fromSceneSettings( const QJsonObject& rclSettings );

protected:
    bool                      m_bOn;
    std::optional<uint8_t>    m_uiBrightness;
    std::optional<LightColor> m_clColor;
    std::optional<int>        m_iTemperature;
};

#endif // LIGHTBULBSTATE_H
