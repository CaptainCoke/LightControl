#ifndef NODETOOLS_H
#define NODETOOLS_H

#include <QString>
#include <QtDebug>
#include <memory>
#include <boost/core/demangle.hpp>

template<typename T>
static QString getTypeName()
{
    return QString::fromStdString(boost::core::demangle(typeid(T).name()));
}

template<typename T>
static std::shared_ptr<T> getByName( const QString& strName )
{
    std::list<std::shared_ptr<T>> lst_matches;
    for ( const auto& [str_id, pcl_node] : T::getAll() )
    {
        std::shared_ptr<T> pcl_element = std::dynamic_pointer_cast<T>(pcl_node);
        if ( pcl_element && pcl_element->name().compare( strName, Qt::CaseInsensitive ) == 0 )
        {
            qDebug() << "found" << getTypeName<T>() << str_id << "for name" << strName;
            lst_matches.emplace_back( std::move(pcl_element) );
        }
    }
    if ( lst_matches.empty() )
        throw std::runtime_error( qPrintable(QString( "%1 with name \"%2\" not found" ).arg(getTypeName<T>(),strName)) );
    else if ( lst_matches.size() > 1 )
        throw std::runtime_error( qPrintable(QString( "%1 name \"%2\" is ambiguous" ).arg(getTypeName<T>(),strName)) );
    else
        return lst_matches.front();
}


#endif // NODETOOLS_H
