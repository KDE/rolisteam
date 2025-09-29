#include "propertiesmodel.h"

PropertiesModel::PropertiesModel(QObject* parent) : QAbstractListModel(parent) {}

int PropertiesModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if(parent.isValid())
        return 0;

    return m_data.size();
}

QVariant PropertiesModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto it= m_data[index.row()];
    QVariant res;
    switch(role)
    {
    case Qt::UserRole + 1: // key
        res= it.key;
        break;
    case Qt::UserRole + 2: // value
        res= it.value;
        break;
    }
    return res;
}

bool PropertiesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    auto& it= m_data[index.row()];
    bool res= false;
    switch(role)
    {
    case Qt::UserRole + 1: // key
        it.key= value.toString();
        break;
    case Qt::UserRole + 2: // value
        it.value= value.toString();
        break;
    }

    emit dataChanged(index, index, {role});
    return res;
}

QHash<int, QByteArray> PropertiesModel::roleNames() const
{
    return {{Qt::UserRole + 1, "key"}, {Qt::UserRole + 2, "value"}};
}

QHash<QString, QString> PropertiesModel::dictionary() const
{
    QHash<QString, QString> hash;
    for(auto const& i : m_data)
        hash.insert(i.key, i.value);
    return hash;
}

const std::vector<FieldInfo>& PropertiesModel::infos() const
{
    return m_data;
}

void PropertiesModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

void PropertiesModel::addField(const QString& key, const QString& value)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(FieldInfo{key, value});
    endInsertRows();
    emit countChanged();
}

void PropertiesModel::removeField(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_data.erase(std::begin(m_data) + index);
    endRemoveRows();
    emit countChanged();
}
