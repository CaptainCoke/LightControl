#ifndef SCENEBUTTONHANDLER_H
#define SCENEBUTTONHANDLER_H

#include "RemoteControlButtonHandler.h"
#include <memory>

class SceneButtonHandler : public RemoteControlButtonHandler
{
public:
    SceneButtonHandler( bool bForward );
    void workOn(LightGroup& rclGroup) override;
    static std::shared_ptr<SceneButtonHandler> nextScene();
    static std::shared_ptr<SceneButtonHandler> previousScene();
private:
    bool m_bForward = true;
};

#endif // SCENEBUTTONHANDLER_H
