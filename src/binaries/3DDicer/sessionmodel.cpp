#include "sessionmodel.h"
#include <memory>
SessionModel::SessionModel(QObject* parent) : QAbstractListModel(parent) {}

int SessionModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_sessions.size();
}

QVariant SessionModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QVariant res;
    auto const& session= m_sessions[index.row()];
    switch(role)
    {
    case NameRole:
        res= session->name();
        break;
    default:
        break;
    }
    return res;
}

bool SessionModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    auto session= m_sessions[index.row()].get();
    bool res= false;
    switch(role)
    {
    case NameRole:
        session->setName(value.toString());
        res= true;
        break;
    default:
        break;
    }

    emit dataChanged(index, index, {role});
    return res;
}

QHash<int, QByteArray> SessionModel::roleNames() const
{
    return {{NameRole, "name"}};
}

Session* SessionModel::session(int index) const
{
    return m_sessions[index].get();
}

QStringList SessionModel::sessionNames() const
{
    QStringList res;
    std::transform(std::begin(m_sessions), std::end(m_sessions), std::back_inserter(res),
                   [](const std::unique_ptr<Session>& session) { return session->name(); });
    return res;
}

void SessionModel::addSession(const QString& name)
{
    beginInsertRows(QModelIndex(), m_sessions.size(), m_sessions.size());
    m_sessions.push_back(std::make_unique<Session>(name));
    endInsertRows();
}

void SessionModel::removeSession(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_sessions.erase(std::begin(m_sessions) + index);
    endRemoveRows();
}
