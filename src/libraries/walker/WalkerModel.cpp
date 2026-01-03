#include "WalkerModel.h"
#include "walkerattachedtype.h"
#include "walkeritem.h"

WalkerModel::WalkerModel(QObject* parent) : QAbstractListModel(parent) {}

int WalkerModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_items.size();
}

QVariant WalkerModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto p= qobject_cast<WalkerAttachedType*>(qmlAttachedPropertiesObject<WalkerItem*>(m_items[index.row()], false));

    QVariant var;
    if(!p)
        return var;

    switch(role)
    {
    case WeightRole:
        var= p->weight();
        break;
    case DescRole:
        var= p->description();
        break;
    }

    return var;
}
