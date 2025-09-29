#include "macromodel.h"

MacrosModel::MacrosModel(QObject* parent) : QAbstractListModel(parent) {}

int MacrosModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_data.size();
}

QVariant MacrosModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto it= m_data[index.row()];
    QVariant res;
    switch(role)
    {
    case NameRole: // key
        res= it.name;
        break;
    case CommandRole: // value
        res= it.command;
        break;
    }
    return res;
}

bool MacrosModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    auto& it= m_data[index.row()];
    bool res= false;
    switch(role)
    {
    case NameRole: // key
        it.name= value.toString();
        break;
    case CommandRole: // value
        it.command= value.toString();
        break;
    }

    emit dataChanged(index, index, {role});
    return res;
}

QHash<int, QByteArray> MacrosModel::roleNames() const
{
    return {{NameRole, "name"}, {CommandRole, "command"}};
}

const std::vector<MacroInfo>& MacrosModel::macros() const
{
    return m_data;
}

void MacrosModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

void MacrosModel::addMacro(const QString& key, const QString& value)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(MacroInfo{key, value});
    endInsertRows();
    emit countChanged();
}

void MacrosModel::removeMacro(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_data.erase(std::begin(m_data) + index);
    endRemoveRows();
    emit countChanged();
}
