#include "rollmodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

RollModel::RollModel(QObject* parent) : QAbstractListModel(parent) {}

int RollModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_rolls.size();
}

QVariant RollModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    static QSet<int> allow{CommandRole, ResultRole, JsonRole, CommentRole, DateTimeRole, DetailRole, Qt::DisplayRole};
    if(!allow.contains(role))
        return {};

    auto const& info= m_rolls[index.row()];
    QVariant res;
    switch(role)
    {
    case Qt::DisplayRole:
    case ResultRole:
        res= info.result;
        break;
    case CommandRole:
        res= info.command;
        break;
    case JsonRole:
        res= info.json;
        break;
    case DateTimeRole:
        res= info.datetime;
        break;
    case CommentRole:
        res= info.comment;
        break;
    case DetailRole:
        res= info.details;
        break;
    default:
        break;
    }

    return res;
}

void RollModel::addRoll(const QByteArray& data)
{
    // qDebug() << "addRoll" << data;
    QJsonDocument doc= QJsonDocument::fromJson(data);
    auto obj= doc.object();
    auto instructions= obj["instructions"].toArray();
    QMap<int, QStringList> detailsResultMap;
    QSet<QString> alreadyDice;
    for(auto inst : std::as_const(instructions))
    {
        auto instObj= inst.toObject();
        auto array= instObj["diceval"].toArray();
        for(auto val : std::as_const(array))
        {
            auto value= val.toObject();
            auto face= value["face"].toInt();
            auto text= value["string"].toString();
            auto id= value["uuid"].toString();
            if(alreadyDice.contains(id))
                continue;
            if(detailsResultMap.contains(face))
            {
                auto& list= detailsResultMap[face];
                list.append(text);
            }
            else
            {
                detailsResultMap[face]= QStringList{text};
            }

            alreadyDice.insert(id);
        }
    }

    QStringList tempRes;
    for(auto [k, v] : detailsResultMap.asKeyValueRange())
    {
        tempRes << QStringLiteral("d%1:(%2)").arg(k).arg(v.join(','));
    }

    beginInsertRows(QModelIndex(), 0, 0);
    m_rolls.prepend(RollInfo{obj["command"].toString(), obj["string"].toString(), data, obj["comment"].toString(),
                             QDateTime::currentDateTime().toString("hh:mm:ss"), tempRes.join(",")});
    endInsertRows();
}

void RollModel::clear()
{
    beginResetModel();
    m_rolls.clear();
    endResetModel();
}

QHash<int, QByteArray> RollModel::roleNames() const
{
    return {{CommandRole, "command"}, {ResultRole, "result"},  {JsonRole, "json"},
            {CommentRole, "comment"}, {DetailRole, "details"}, {DateTimeRole, "time"}};
}
