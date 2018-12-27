#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QTimer>
#include <QString>

class Node : public QObject
{
    Q_OBJECT
public:
    ~Node() override;

    const QString& id() const { return m_strId; }
    const QString& name() const { return m_strName; }

    virtual void setNodeData(const QJsonObject &rclObject);

    virtual const QString& nodeType() const = 0;
    virtual bool isEmpty() const { return false; }

    QString uniqueId() const { return nodeType() + id(); }

signals:
    void stateChanged();
    void nodeDeleted(QString);

public slots:
    void refreshNode();
    void deleteNode();

protected:
    Node() = default; // constructor is default. All initialization work is done by "initializeNode".

    virtual void initializeNode(const QString& strId);
    void refreshPeriodically(uint32_t uiMilliseconds);

private:
    QString m_strId;
    QString m_strName;
    QString m_strEtag;

    QTimer  m_clStatePollingTimer;
};

#endif // NODE_H
