#ifndef DIMMABLELIGHTBULB_H
#define DIMMABLELIGHTBULB_H

#include "LightBulb.h"

class DimmableLightBulb : public LightBulb
{
public:
    ~DimmableLightBulb() override = default;

    uint8_t brightness() const;

    static bool isDimmableLight( const QJsonObject &rclObject );

public slots:
    void setBrightness( uint8_t uiBrightness, float fTransitionTimeS = 0.f );

protected:
    using LightBulb::LightBulb;
};

#endif // DIMMABLELIGHTBULB_H
