#ifndef DEVICENODE_H
#define DEVICENODE_H

#include <QString>
#include <QObject>

class DeviceNode : public QObject
{
    Q_OBJECT
public:
    ~DeviceNode() override;

    const QString& id() const { return m_strId; }
    const QString& name() const { return m_strName; }
    const QString& manufacturer() const { return m_strManufacturer; }
    const QString& modelID() const { return m_strModelID; }
    const QString& SWVersion() const { return m_strSWVersion; }

    bool isReachable() const { return m_bReachable; }

    virtual void setNodeData(const QJsonObject &rclObject);

    virtual QString nodeType() const = 0;

    QString uniqueId() const { return nodeType() + id(); }

signals:
    void stateChanged();
    void nodeDeleted(QString);

public slots:
    void refreshNode();
    void deleteNode();

protected:
    DeviceNode(const QString& strId);
    bool setReachable( bool bReachable );
    void changeState( QJsonObject clObject, float fTransitionTimeS = 0.f );

private:
    QString m_strId;
    QString m_strName;
    QString m_strManufacturer;
    QString m_strModelID;
    QString m_strSWVersion;
    QString m_strEtag;
    bool    m_bReachable;

};

#endif // DEVICENODE_H
