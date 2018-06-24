#ifndef LIGHTCOLOR_H
#define LIGHTCOLOR_H

#include <cstdint>

class LightTemperature;

class LightColor
{
public:
    static LightColor fromXY( double x, double y );

    /// hue in [0,359], saturation in [0,255] and V in [0,255]
    static LightColor fromHSV( int h, uint8_t s, uint8_t v );

    static LightColor fromTemperature( LightTemperature temperature );

    double x() const { return m_x; }
    double y() const { return m_y; }

    // get saturation and hue for luminance Y
    uint8_t saturation( double Y ) const;
    uint8_t saturation( uint8_t Y ) const;
    int hue( double Y ) const;
    int hue( uint8_t Y ) const;
    LightTemperature temperature() const;

    bool operator!=(const LightColor& rclOther) const;
    bool operator==(const LightColor& rclOther) const;

private:
    LightColor();

    // store color internally as "xy"
    double m_x, m_y;
};

#endif // LIGHTCOLOR_H
