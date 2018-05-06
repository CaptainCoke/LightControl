#ifndef CTLIGHTBULB_H
#define CTLIGHTBULB_H

#include "LightBulb.h"
#include "LightTemperature.h"

class CTLightBulb : public LightBulb
{
public:
    ~CTLightBulb() override = default;

    LightTemperature minTemperature() const { return m_clMinTemperature; }
    LightTemperature maxTemperature() const { return m_clMaxTemperature; }
    LightTemperature temperature() const { return m_clTemperature; }

    static bool isCTLight( const QJsonObject &rclObject );

    void setNodeData(const QJsonObject &rclObject) override;
    void setToState( const LightBulbState& rclState ) override;
    void setTemperature( LightTemperature clTemperature, float fTransitionTimeS = 0.f );
    LightBulbState getCurrentState() const override;

public slots:
    void setMired( uint16_t uiMired, float fTransitionTimeS = 0.f );

protected:
    using LightBulb::LightBulb;
    bool setStateData(const QJsonObject &rclObject) override;

private:
    LightTemperature m_clTemperature = LightTemperature::fromKelvin(2700), m_clMinTemperature = LightTemperature::fromKelvin(2700), m_clMaxTemperature = LightTemperature::fromKelvin(2700);
};

#endif // CTLIGHTBULB_H
