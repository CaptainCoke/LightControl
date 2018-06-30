#include "LightControl.h"
#include "ui_LightControl.h"
#include <QJsonObject>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QBoxLayout>
#include "Widgets/SensorWidget.h"
#include "Widgets/LightBulbWidget.h"
#include "Widgets/LightGroupWidget.h"
#include "Nodes/LightBulb.h"
#include "Nodes/Sensor.h"
#include "Nodes/LightGroup.h"
#include "GatewayAccess.h"

LightControl::LightControl(QWidget *parent)
: QMainWindow(parent)
, m_pclUI(std::make_unique<Ui::LightControl>())
{
    m_pclUI->setupUi(this);
    m_pclUI->lightsWidget->setLayout( new QVBoxLayout() );
    m_pclUI->sensorsWidget->setLayout( new QVBoxLayout() );
    m_pclUI->groupsWidget->setLayout( new QVBoxLayout() );
    connect( &GatewayAccess::instance(), &GatewayAccess::networkInfo, [this](const QString& strMessage){
        m_pclUI->statusBar->showMessage(strMessage);
    } );
    connect( &GatewayAccess::instance(), &GatewayAccess::networkError, [this](const QString& strMessage){
        m_pclUI->statusBar->showMessage(strMessage);
        QMessageBox::critical(this, "Network Error", strMessage);
    } );
}

LightControl::~LightControl() = default;

void LightControl::removeWidget(QString strUniqueId)
{
    auto it_widget = m_mapNodeWidgets.find(strUniqueId);
    if ( it_widget != m_mapNodeWidgets.end() )
    {
        QWidget* pcl_widget = it_widget->second;
        QWidget* pcl_parent_widget = dynamic_cast<QWidget*>(pcl_widget->parent());
        pcl_parent_widget->layout()->removeWidget(pcl_widget);
        delete pcl_widget;
        m_mapNodeWidgets.erase( it_widget );
    }
}

template<class NodeFactory,class WidgetFactory>
QStringList LightControl::updateWidget(QLayout* pclLayout)
{
    QStringList lst_widget_ids;
    for ( const auto& [str_node_id, pcl_node] : NodeFactory::getAll() )
    {
        (void)str_node_id;
        QString str_widget_id = pcl_node->uniqueId();
        lst_widget_ids << str_widget_id;
        auto it_widget = m_mapNodeWidgets.find( str_widget_id );
        if ( it_widget == m_mapNodeWidgets.end() )
        {
            auto* pcl_widget = WidgetFactory::createWidget(pcl_node);
            pclLayout->addWidget(pcl_widget);
            m_mapNodeWidgets[str_widget_id] = pcl_widget;
            connect( pcl_node.get(), &Node::nodeDeleted, this, &LightControl::removeWidget );
        }
    }
    return lst_widget_ids;
}

void LightControl::updateWidgets()
{
    QStringList lst_widget_ids;
    lst_widget_ids.append( updateWidget<LightBulb,LightBulbWidget>(m_pclUI->lightsWidget->layout()) );
    lst_widget_ids.append( updateWidget<Sensor,SensorWidget>(m_pclUI->sensorsWidget->layout()) );
    lst_widget_ids.append( updateWidget<LightGroup,LightGroupWidget>(m_pclUI->groupsWidget->layout()) );

    QStringList lst_removed_widgets;
    for ( const auto&[str_widget_id,pcl_widget] : m_mapNodeWidgets )
    {
        (void)pcl_widget;
        if ( !lst_widget_ids.contains( str_widget_id ) )
            lst_removed_widgets <<str_widget_id;
    }
    for ( const QString &str_widget_id : lst_removed_widgets )
        removeWidget(str_widget_id);
    for ( const auto&[key,pcl_widget] : m_mapNodeWidgets )
    {
        (void)key;
        pcl_widget->updateNode();
    }
}
