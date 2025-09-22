#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

#include <QAbstractListModel>

#include "session.h"

class SessionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CustomRole
    {
        NameRole= Qt::UserRole + 1,
        AliasesRole,
        SheetRole,
        MacrosRole
    };
    Q_ENUM(CustomRole)
    explicit SessionModel(QObject* parent= nullptr);
    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Session* session(int index) const;
    QStringList sessionNames() const;
public slots:
    void addSession(const QString& name= QString());
    void removeSession(int index);

signals:
    void sessionAdded(int index);

private:
    std::vector<std::unique_ptr<Session>> m_sessions;
};

#endif // SESSIONMODEL_H
