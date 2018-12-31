#include "DimmableLightBulb.h"
#include <QJsonObject>
#include "LightBulbState.h"

uint8_t DimmableLightBulb::brightness() const
{
    if ( getCurrentState().hasBrightness() )
        return getCurrentState().brightness();
    else
        return 0;
}

bool DimmableLightBulb::isDimmableLight(const QJsonObject & rclObject)
{
    QJsonObject cl_state = rclObject.value("state").toObject();
    if ( cl_state.isEmpty() )
        return false;
    return LightBulbState::fromSceneSettings(cl_state).hasBrightness();
}

void DimmableLightBulb::setBrightness(uint8_t uiBrightness, float fTransitionTimeS )
{
    setTargetState( LightBulbState( getTargetState() ).setBrightness(uiBrightness), fTransitionTimeS );
    changeToTargetStateIfNecessary();
}
