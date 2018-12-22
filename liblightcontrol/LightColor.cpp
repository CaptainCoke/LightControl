#include "LightColor.h"
#include <cmath>
#include <array>
#include <tuple>
#include <algorithm>
#include "LightTemperature.h"

static std::array<int,3> RGBToHSV( const std::array<double,3>& RGB )
{
    auto [it_min, it_max] = std::minmax_element(RGB.begin(),RGB.end());
    double f_diff = *it_max-*it_min;
    int h, s, v = static_cast<int>(255 * *it_max);
    if (f_diff == 0) {
        // R == G == B
        h = 0;
        s = 0;
    } else {
        auto i_max_element = std::distance( RGB.begin(), it_max );
        auto i_next = (i_max_element+1)%3;
        auto i_next2 = (i_max_element+2)%3;
        h = static_cast<int>(60*(2*i_max_element + (RGB[i_next]-RGB[i_next2])/f_diff));
        s = static_cast<int>(255*(f_diff / *it_max));
    }
    return {h,s,v};
}

static std::array<double,3> HSVToRGB( const std::array<int,3>& HSV )
{
    double h = static_cast<double>(HSV[0]);
    double s = static_cast<double>(HSV[1]) / 255.0;
    double v = static_cast<double>(HSV[2]) / 255.0;

    int h_i = static_cast<int>(std::floor(h/60.0));
    double f = h_i - h/60.0;
    
    double p = v*(1-s);
    double q = v*(1-s*f);
    double t = v*(1-s*(1-f));

    switch(h_i) {
    case 6: [[fallthrough]];
    case 0: return {v,t,p};
    case 1: return {q,v,p};
    case 2: return {p,v,t};
    case 3: return {p,q,v};
    case 4: return {t,p,v};
    case 5: return {v,p,q};
    default: throw std::runtime_error( "invalid hue value" );
    }
}

static std::array<int,3> YxyToHSV(const std::array<double,3>& Yxy)
{
    const auto& [Y,x,y] = Yxy;

    double X = Y/y * x;
    double Z = Y/y * (1.0-x-y);

    double r =  1.612 * X - 0.203 * Y - 0.302 * Z;
    double g = -0.509 * X + 1.412 * Y + 0.066 * Z;
    double b =  0.026 * X - 0.072 * Y + 0.962 * Z;
    r = r <= 0.0031308 ? 12.92 * r : (1.0 + 0.055) * std::pow(r, (1.0 / 2.4)) - 0.055;
    g = g <= 0.0031308 ? 12.92 * g : (1.0 + 0.055) * std::pow(g, (1.0 / 2.4)) - 0.055;
    b = b <= 0.0031308 ? 12.92 * b : (1.0 + 0.055) * std::pow(b, (1.0 / 2.4)) - 0.055;

    double f_max = std::max(r,std::max(g,b));
    if ( f_max != 0 )
    {
        r /= f_max;
        g /= f_max;
        b /= f_max;
        if (r < 0) r = 1.0;
        if (g < 0) g = 1.0;
        if (b < 0) b = 1.0;
    }
    else
    {
        r = g = b = 0;
    }
    return RGBToHSV({r,g,b});
}

static std::array<double,3> HSVToYxy(const std::array<int,3>& HSV)
{
    auto [r,g,b] = HSVToRGB( HSV );
    r = r > 0.04045 ? std::pow( r + 0.055 / (1.0 + 0.055), 2.4 ) : ( r / 12.92 );
    g = g > 0.04045 ? std::pow( g + 0.055 / (1.0 + 0.055), 2.4 ) : ( g / 12.92 );
    b = b > 0.04045 ? std::pow( b + 0.055 / (1.0 + 0.055), 2.4 ) : ( b / 12.92 );

    double X = 0.649926 * r + 0.103455 * g + 0.197109 * b;
    double Y = 0.234327 * r + 0.743075 * g + 0.022598 * b;
    double Z = 0.000000 * r + 0.053077 * g + 1.035763 * b;

    double f_sum = X+Y+Z, x, y;
    if ( f_sum == 0 )
    {
        x = y = 0;
    }
    else
    {
        x = X / f_sum;
        y = Y / f_sum;
    }
    return {Y,x,y};
}

static std::pair<double,double> kelvinToxy( uint16_t uiKelvin )
{
    // using cubic spline approximation by Kim et al.
    // https://en.wikipedia.org/wiki/Planckian_locus#Approximation

    double T_inv  = 1e3/static_cast<double>(uiKelvin);
    double T2_inv = T_inv*T_inv;
    double T3_inv = T2_inv*T_inv;

    double x,y;
    if ( uiKelvin <= 4000 )
        x = -0.2661239*T3_inv - 0.2343580*T2_inv + 0.8776856*T_inv + 0.179910;
    else
        x = -3.0258469*T3_inv + 2.1070379*T2_inv + 0.2226347*T_inv + 0.240390;

    double x2=x*x;
    double x3=x2*x;

    if ( uiKelvin <= 2222 )
        y = -1.1063814*x3 - 1.34811020*x2 + 2.18555832*x - 0.20219683;
    else if ( uiKelvin <= 4000 )
        y = -0.9549476*x3 - 1.37418593*x2 + 2.09137015*x - 0.16748867;
    else
        y = 3.0817580*x3 - 5.87338670*x2 + 3.75112997*x - 0.37001483;

    return {x,y};
}

static double xyToKelvin( std::pair<double,double> xy )
{
    // using  cubic approximation by McCamy
    // https://en.wikipedia.org/wiki/Color_temperature#Approximation
    const auto& [x,y] = xy;
    static const double x_e = 0.3320, y_e = 0.1858;
    double n = (x-x_e)/(y-y_e);
    double n2 = n*n;
    double n3 = n2*n;

    return -449*n3 + 3525*n2 - 6823.3*n + 5520.33;
}

LightColor::LightColor() = default;

bool LightColor::operator !=( const LightColor& rclOther) const
{
    return std::abs(m_x - rclOther.m_x) > 1e-4 || std::abs(m_y - rclOther.m_y) >1e-4;
}

bool LightColor::operator==(const LightColor &rclOther) const
{
    return !operator!=(rclOther);
}

LightColor LightColor::fromXY( double x, double y )
{
    LightColor cl_col;
    cl_col.m_x = x;
    cl_col.m_y = y;
    return cl_col;
}

LightColor LightColor::fromHSV( int h, uint8_t s, uint8_t v )
{
    auto [Y,x,y] = HSVToYxy( std::array<int,3>{h,s,v} );
    (void)Y;
    return fromXY( x,y );
}

LightColor LightColor::fromTemperature(LightTemperature temperature)
{
    LightColor cl_col;
    std::tie( cl_col.m_x, cl_col.m_y ) = kelvinToxy(temperature.kelvin());
    return cl_col;
}

uint8_t LightColor::saturation(double Y) const
{
    return static_cast<uint8_t>(YxyToHSV( {Y,m_x,m_y} )[1]);
}

int LightColor::hue(double Y) const
{
    return YxyToHSV( {Y,m_x,m_y} )[0];
}

uint8_t LightColor::saturation(uint8_t Y) const
{
    return saturation( static_cast<double>(Y)/255.0 );
}

int LightColor::hue(uint8_t Y) const
{
    return hue( static_cast<double>(Y)/255.0 );
}

LightTemperature LightColor::temperature() const
{
    return LightTemperature::fromKelvin( static_cast<uint16_t>(xyToKelvin( {m_x,m_y} )) );
}
