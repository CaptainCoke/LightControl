#ifndef LIGHTTEMPERATURE_H
#define LIGHTTEMPERATURE_H

#include <cstdint>

class LightTemperature
{
public:
    static LightTemperature fromKelvin( uint16_t kelvin );
    static LightTemperature fromMired( uint16_t mired );

    uint16_t mired() const { return m_mired; }
    uint16_t kelvin() const;

    bool operator!=(const LightTemperature& rclOther) const;

private:
    LightTemperature();

    // store temperature internally as "mired"
    uint16_t m_mired;
};

#endif // LIGHTTEMPERATURE_H
