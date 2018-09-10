#ifndef LIGHTSTATETRANSITION_H
#define LIGHTSTATETRANSITION_H

#include <memory>
#include <QDateTime>
#include <QMetaObject>
#include <QTimer>

class LightBulb;

class LightStateTransition : public std::enable_shared_from_this<LightStateTransition>
{
public:
    LightStateTransition( std::shared_ptr<LightBulb> pclLight, double fTransitionTimeSeconds = 15 );

    void start();
    void abort();
    bool isFinished();

    static void abortIfExists( const QString& strLightId );

protected:
    void step();
    void finish();
    void cleanup();
    void abortIfPowerOff();

    void setValueAtPosition( double fAlpha );

private:
    enum LightAttribute { Power, Brightness, Temperature, NUM_ATTRIBUTES };
    LightAttribute m_eNextAttribute = Brightness;

    std::shared_ptr<LightBulb> m_pclLight;
    bool m_bFinished = true;
    QDateTime m_tStart;
    double m_fTransitionTimeSeconds;

    double m_fMiredStart = 454, m_fMiredEnd = 250;
    double m_fMinMired = 250, m_fMaxMired = 454;
    double m_fBrightnessStart = 1, m_fBrightnessEnd = 255;

    QMetaObject::Connection m_clStepConnection, m_clAbortConnection;
    QTimer m_clNextEventTimer;

    static std::map<QString,std::shared_ptr<LightStateTransition>> s_mapAllLightStateTransitions;
};


#endif // LIGHTSTATETRANSITION_H
