#ifndef ROLLMODEL_H
#define ROLLMODEL_H

#include <QAbstractListModel>

struct RollInfo
{
    QString command;
    QString result;
    QByteArray json;
    QString comment;
    QString datetime;
    QString details;
};

class RollModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CustomRole
    {
        CommandRole= Qt::UserRole + 1,
        ResultRole,
        JsonRole,
        CommentRole,
        DateTimeRole,
        DetailRole
    };
    Q_ENUM(CustomRole)
    explicit RollModel(QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    void clear();
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void addRoll(const QByteArray& data);

private:
    std::vector<RollInfo> m_rolls;
};

#endif // ROLLMODEL_H
