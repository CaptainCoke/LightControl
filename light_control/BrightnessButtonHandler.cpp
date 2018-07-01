#include "BrightnessButtonHandler.h"
#include <QtDebug>
#include "Nodes/LightGroup.h"
#include "Nodes/LightBulb.h"

BrightnessButtonHandler::BrightnessButtonHandler(int8_t iBrightnessStep)
    : m_iBrightnessStep(iBrightnessStep)
{}

void BrightnessButtonHandler::workOn(LightGroup &rclGroup)
{
    if ( rclGroup.anyOn() )
    {
        float f_transition_time_secs = 0.5f;

        qDebug() << "changing brightness of group" << rclGroup.name() << "by" << m_iBrightnessStep;
        for ( const auto &pcl_light : rclGroup.lights() )
            pcl_light->setBrightness(std::max(1,std::min(255,pcl_light->brightness()+m_iBrightnessStep)),f_transition_time_secs);
    }
}
