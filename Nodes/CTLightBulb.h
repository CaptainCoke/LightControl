#ifndef CTLIGHTBULB_H
#define CTLIGHTBULB_H

#include "LightBulb.h"

class CTLightBulb : public LightBulb
{
public:
    ~CTLightBulb() override = default;

    int minTemperature() const { return m_iMinTemperature; }
    int maxTemperature() const { return m_iMaxTemperature; }
    int temperature() const { return m_iTemperature; }

    static bool isCTLight( const QJsonObject &rclObject );

    void setNodeData(const QJsonObject &rclObject) override;

public slots:
    void setTemperature( int iTemperature, float fTransitionTimeS = 0.f );

protected:
    using LightBulb::LightBulb;
    bool setStateData(const QJsonObject &rclObject) override;

private:
    int m_iTemperature, m_iMinTemperature, m_iMaxTemperature;
};

#endif // CTLIGHTBULB_H
