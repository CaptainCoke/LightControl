#include "CTLightBulb.h"
#include <QJsonObject>

bool CTLightBulb::isCTLight(const QJsonObject &rclObject)
{
    if ( !rclObject.value("hascolor").isBool() || !rclObject.value("hascolor").toBool() )
        return false;
    QJsonObject cl_state = rclObject.value("state").toObject();
    if ( cl_state.isEmpty() || cl_state.find("ct") == cl_state.end() )
        return false;
    return true;
}
