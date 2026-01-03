#ifndef WALKERATTACHEDTYPE_H
#define WALKERATTACHEDTYPE_H

#include <QObject>
#include <QQuickItem>

#include <qqmlintegration.h>

class WalkerAttachedType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged FINAL)
    Q_PROPERTY(int weight READ weight WRITE setWeight NOTIFY weightChanged FINAL)
    QML_ANONYMOUS
public:
    explicit WalkerAttachedType(QObject* parent= nullptr);

    QString description() const;
    void setDescription(const QString& newDescription);

    int weight() const;
    void setWeight(int newWeight);

signals:
    void enter();
    void exit();
    void descriptionChanged();
    void weightChanged();

private:
    QString m_description;
    int m_weight;
};

#endif // WALKERATTACHEDTYPE_H
