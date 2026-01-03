#ifndef WALKERMODEL_H
#define WALKERMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QPointer>
#include <QQuickItem>

class WalkerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum EnumRole
    {
        WeightRole= Qt::UserRole + 1,
        DescRole,
    };

    explicit WalkerModel(QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    void appendData(QQuickItem* item);

private:
    QList<QPointer<QQuickItem>> m_items;
};

#endif // WALKERMODEL_H
