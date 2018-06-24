#include "LightTemperature.h"

LightTemperature::LightTemperature() = default;

bool LightTemperature::operator !=( const LightTemperature& rclOther) const
{
    return m_mired != rclOther.m_mired;
}

bool LightTemperature::operator==(const LightTemperature &rclOther) const
{
    return !operator!=(rclOther);
}

LightTemperature LightTemperature::fromMired(uint16_t mired)
{
    LightTemperature cl_temp;
    cl_temp.m_mired = mired;
    return cl_temp;
}

uint16_t LightTemperature::kelvin() const
{
    return static_cast<uint16_t>(1e6 / static_cast<double>(m_mired));
}

LightTemperature LightTemperature::fromKelvin(uint16_t kelvin)
{
    LightTemperature cl_temp;
    cl_temp.m_mired = static_cast<uint16_t>(1e6 / static_cast<double>(kelvin));
    return cl_temp;
}
