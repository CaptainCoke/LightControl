#include "RGBLightBulb.h"
#include <cmath>
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>

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

    QColor cl_color;
    cl_color.setRgbF( r, g, b );
    int h, s, v;
    cl_color.getHsv(&h,&s,&v);
    return {h,s,v};
}

static std::array<double,3> HSVToYxy(const std::array<int,3>& HSV)
{
    const auto & [h,s,v] = HSV;
    QColor cl_hsv;
    cl_hsv.setHsv(h,s,v);
    double r,g,b;
    cl_hsv.getRgbF(&r,&g,&b);
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

bool RGBLightBulb::setStateData(const QJsonObject &rclObject)
{
    bool b_changed = LightBulb::setStateData(rclObject);
    QJsonArray arr_xy = rclObject.value("xy").toArray();
    if ( arr_xy.isEmpty() )
    {
        int H = rclObject.value("hue").toInt();
        int S = rclObject.value("sat").toInt();
        b_changed = b_changed || S != m_uiSaturation || H != m_iHue;
        m_uiSaturation = static_cast<uint8_t>(S);
        m_iHue         = H;
    }
    else
    {
        double Y = static_cast<double>(brightness())/255.0;
        auto [h,s,v] = YxyToHSV({Y,arr_xy[0].toDouble(),arr_xy[1].toDouble()});
        (void)v;
        b_changed = b_changed || s != m_uiSaturation || h != m_iHue;
        m_uiSaturation = static_cast<uint8_t>(s);
        m_iHue         = h;
    }
    return b_changed;
}

void RGBLightBulb::setColor(float fTransitionTimeS)
{
    auto [Y,x,y] = HSVToYxy( {hue(), saturation(), brightness()} );
    (void)Y;
    QJsonArray xy = {x,y};
    QJsonObject cl_object{ {"xy", xy } };
    changeState(std::move(cl_object),fTransitionTimeS);
}

bool RGBLightBulb::isRGBLight(const QJsonObject &rclObject)
{
    if ( !rclObject.value("hascolor").isBool() || !rclObject.value("hascolor").toBool() )
        return false;
    QJsonObject cl_state = rclObject.value("state").toObject();
    if ( cl_state.isEmpty() || cl_state.find("sat") == cl_state.end() )
        return false;
    return true;
}

void RGBLightBulb::setHue(int iHue, float fTransitionTimeS)
{
    iHue = std::clamp( iHue, 0, 360 );
    if ( iHue != m_iHue )
    {
        m_iHue = iHue;
        setColor( fTransitionTimeS );
        emit stateChanged();
    }
}

void RGBLightBulb::setSaturation(uint8_t uiSaturation, float fTransitionTimeS)
{
    if ( uiSaturation != m_uiSaturation )
    {
        m_uiSaturation = uiSaturation;
        setColor( fTransitionTimeS );
        emit stateChanged();
    }
}
