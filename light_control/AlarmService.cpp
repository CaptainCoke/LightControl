#include "AlarmService.h"
#include <QSettings>
#include <QtDebug>
#include <QTime>
#include <set>
#include "Nodes/LightBulb.h"
#include "Nodes/LightGroup.h"
#include "Nodes/LightGroupScene.h"
#include "NodeTools.h"
#include "LightBulbState.h"
#include "LightStateTransition.h"

class AlarmService::Alarm
{
public:
    Alarm( QString strGroupName, QString strSceneName, QTime clTime, double fDuration, std::set<Qt::DayOfWeek> setWeekdays )
    : m_strGroupName( std::move(strGroupName) )
    , m_strSceneName( std::move(strSceneName) )
    , m_clTime( std::move(clTime) )
    , m_fDuration(fDuration)
    , m_setWeekdays( std::move(setWeekdays) )
    {}

    void activate()
    {
        auto today = static_cast<Qt::DayOfWeek>(QDate::currentDate().dayOfWeek());
        if ( !m_setWeekdays.empty() && m_setWeekdays.find( today ) == m_setWeekdays.end() )
            return;
        auto seconds_to_alarm = QTime::currentTime().secsTo( m_clTime );
        if ( seconds_to_alarm < 0 || seconds_to_alarm > 10 )
            return;
        auto seconds_since_last_active = m_clLastActivated.secsTo( QDateTime::currentDateTime() );
        if ( m_clLastActivated.isValid() && seconds_since_last_active < 60 )
            return;

        //activate timer
        m_clLastActivated = QDateTime::currentDateTime();

        auto group = getByName<LightGroup>(m_strGroupName);
        if ( !group )
        {
            qCritical() << "cannot activate timer for group"<<m_strGroupName<<": group not found!";
            return;
        }
        if ( group->anyOn() )
        {
            qInfo() << "skipping alarm event at"<<m_clTime.toString("hh:mm")<<"on group"<<m_strGroupName<<"due to (some) lights being on already";
            return;
        }
        for ( const auto &[str_group_id,pcl_scene] : group->scenes() )
        {
            if ( m_strSceneName.compare( pcl_scene->name(), Qt::CaseInsensitive ) == 0 )
            {                    
                qInfo() << "activating lights of scene"<<m_strSceneName<<"of group"<<m_strGroupName<<"due to alarm event at"<<m_clTime.toString("hh:mm");
                for ( const auto &[str_light_id,cl_state] : pcl_scene->getStates() )
                {
                    if ( cl_state.on() )
                    {
                        LightStateTransition::abortIfExists( str_light_id );
                        auto pcl_transition = std::make_shared<LightStateTransition>( LightBulb::get(str_light_id), m_fDuration );
                        pcl_transition->start();
                    }
                }
                return;
            }
        }
        qCritical() << "cannot activate timer for group"<<m_strGroupName<<": scene"<<m_strSceneName<<"not found!";
    }
private:
    QString m_strGroupName, m_strSceneName;
    QTime m_clTime;
    QDateTime m_clLastActivated;
    double m_fDuration;
    std::set<Qt::DayOfWeek> m_setWeekdays;
};

AlarmService::AlarmService(QObject *parent)
: QObject(parent)
{
    connect( &m_clPeriodicCheckTimer, &QTimer::timeout, this, &AlarmService::handleAlarms );
    m_clPeriodicCheckTimer.setInterval(1000);
    m_clPeriodicCheckTimer.start();
}

AlarmService::~AlarmService() = default;

void AlarmService::start()
{
    loadAlarms();
}

void AlarmService::stop()
{
}

void AlarmService::handleAlarms()
{
    for ( auto & rcl_alarm : m_lstAlarms )
        rcl_alarm.activate();
}

void AlarmService::interruptAlarmsOnExternalChange()
{
    if ( auto pcl_calling_group = dynamic_cast<LightGroup*>(sender()); pcl_calling_group != nullptr )
    {
        // abort all running transitions
        for ( const auto & pcl_light : pcl_calling_group->lights() )
            LightStateTransition::abortIfExists( pcl_light->id() );
    }
}

static std::set<Qt::DayOfWeek> parseWeekdays( const QString& strWeekdays )
{
    static std::map<QString,Qt::DayOfWeek> map_days{ {"Mon",Qt::Monday},{"Tue",Qt::Tuesday},{"Wed",Qt::Wednesday},{"Thu",Qt::Thursday},{"Fri",Qt::Friday},{"Sat",Qt::Saturday},{"Sun",Qt::Sunday}};
    std::set<Qt::DayOfWeek> set_days;
    for ( const QString& str_day : strWeekdays.split(",") )
    {
        auto it_day = map_days.find(str_day);
        if ( it_day != map_days.end() )
            set_days.insert(it_day->second);
        else
            qCritical() << "weekday" << str_day << " cannot be interpreted";
    }
    return set_days;
}

void AlarmService::loadAlarms()
{
    QSettings cl_settings;
    int num_alarms = cl_settings.beginReadArray("alarm");
    std::set<QString> set_groups_with_alarms;
    for (int i = 0; i < num_alarms; ++i)
    {
        cl_settings.setArrayIndex(i);

        QString str_group_name = cl_settings.value("group").toString();
        QString str_scene_name = cl_settings.value("scene").toString();
        QString str_timepoint  = cl_settings.value("time").toString();
        QString str_weekdays   = cl_settings.value("weekdays").toString();
        double f_duration      = cl_settings.value("duration").toDouble();

        QTime cl_time;
        if ( str_timepoint.compare("now", Qt::CaseInsensitive) == 0 )
            cl_time = QTime::currentTime().addSecs(2);
        else
            cl_time = QTime::fromString( str_timepoint, "hh:mm" );
        if ( !cl_time.isValid() )
        {
            qCritical() << "timepoint" << str_timepoint << "is invalid";
            continue;
        }
        std::set<Qt::DayOfWeek> set_weekdays = parseWeekdays(str_weekdays);

        qInfo() << "adding alarm " << str_timepoint << " for" << str_group_name << "scene" << str_scene_name << "on" << str_weekdays;
        set_groups_with_alarms.insert( str_group_name );
        m_lstAlarms.emplace_back( std::move(str_group_name), std::move(str_scene_name), std::move(cl_time), f_duration, std::move(set_weekdays) );
    }
    cl_settings.endArray();

    for ( const QString& str_group_name : set_groups_with_alarms )
    {
        if ( auto pcl_group = getByName<LightGroup>(str_group_name); pcl_group != nullptr )
            connect( pcl_group.get(), &LightGroup::sceneCalled, this, &AlarmService::interruptAlarmsOnExternalChange );
    }
}
