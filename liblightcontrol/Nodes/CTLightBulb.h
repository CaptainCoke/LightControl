#ifndef CTLIGHTBULB_H
#define CTLIGHTBULB_H

#include "DimmableLightBulb.h"
#include "LightTemperature.h"

class CTLightBulb : public DimmableLightBulb
{
public:
    ~CTLightBulb() override = default;

    LightTemperature minTemperature() const { return m_clMinTemperature; }
    LightTemperature maxTemperature() const { return m_clMaxTemperature; }
    LightTemperature temperature() const;

    static bool isCTLight( const QJsonObject &rclObject );

    void setNodeData(const QJsonObject &rclObject) override;
    void setTemperature( LightTemperature clTemperature, float fTransitionTimeS = 0.f );

public slots:
    void setMired( uint16_t uiMired, float fTransitionTimeS = 0.f );

protected:
    using DimmableLightBulb::DimmableLightBulb;
    bool isCloseEnoughToTargetState() const override;

private:
    uint8_t m_uiTemperatureImprecisionMired = 0;
    LightTemperature m_clMinTemperature = LightTemperature::fromKelvin(2700), m_clMaxTemperature = LightTemperature::fromKelvin(2700);
    static std::map<QString,uint8_t> s_mapModelTemperatureImprecisions;
};

#endif // CTLIGHTBULB_H
