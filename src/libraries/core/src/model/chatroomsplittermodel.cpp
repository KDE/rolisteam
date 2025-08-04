/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "model/chatroomsplittermodel.h"
#include "model/filterinstantmessagingmodel.h"
#include "model/instantmessagingmodel.h"
#include <set>
#include "common/logcategory.h"

namespace InstantMessaging
{
ChatroomSplitterModel::ChatroomSplitterModel(QObject* parent) : QAbstractListModel(parent) {}

int ChatroomSplitterModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_filterModels.size());
}

QVariant ChatroomSplitterModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    std::set<int> acceptedRole({FilterModelRole, UuidRole});
    auto it= acceptedRole.find(role);
    if(it == acceptedRole.end())
        return QVariant();

    QVariant var;
    auto model= m_filterModels.at(static_cast<std::size_t>(index.row())).get();
    switch(role)
    {
    case FilterModelRole:
        var= QVariant::fromValue(model);
        break;
    case UuidRole:
        var= model->uuid();
        break;
    case IndexRole:
        var= index.row();
        break;
    }

    return var;
}

QHash<int, QByteArray> ChatroomSplitterModel::roleNames() const
{
    static QHash<int, QByteArray> roles({{FilterModelRole, "filterModel"}, {UuidRole, "uuid"}, {IndexRole,"indexRole"}});
    return roles;
}

void ChatroomSplitterModel::addFilterModel(InstantMessaging::InstantMessagingModel* sourceModel, QStringList list,
                                           bool all)
{
    std::for_each(m_filterModels.begin(), m_filterModels.end(),
                  [list](const std::unique_ptr<FilterInstantMessagingModel>& model)
                  {
                      if(model->all())
                          model->setFilterParameter(true, list);
                  });

    std::unique_ptr<InstantMessaging::FilterInstantMessagingModel> model(new FilterInstantMessagingModel());
    model->setFilterParameter(all, list);

    model->setSourceModel(sourceModel);

    auto size= static_cast<int>(m_filterModels.size());

    beginInsertRows(QModelIndex(), size, size);
    m_filterModels.push_back(std::move(model));
    endInsertRows();
}

void ChatroomSplitterModel::removeModel(int modelIndex)
{
    if(m_filterModels.empty() || modelIndex >= static_cast<int>(m_filterModels.size()))
        return;

    beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
    m_filterModels.erase(std::begin(m_filterModels) + modelIndex);
    endRemoveRows();
}

void ChatroomSplitterModel::mergeGlobal(const QString& uuid, int modelIndex)
{
    if(static_cast<int>(m_filterModels.size()) <= modelIndex)
        return;

    auto model = m_filterModels[modelIndex].get();

    model->removeFilterId(uuid);
    if(model->filterIdCount() == 0)
        removeModel(modelIndex);
    m_filterModels[0]->removeFilterId(uuid);
}

void ChatroomSplitterModel::cleanAll()
{
    beginResetModel();
    m_filterModels.clear();
    endResetModel();
}

void ChatroomSplitterModel::removeChatroom(const QString& id)
{
    //Q_UNUSED(id)
    QList<int> removeModelIndexes;
    QStringList listIds;
    static QString global("global");
    int globalIndex = -1;
    int i = 0;
    for(auto& model : m_filterModels)
    {
        if(i == 0)
        {
            ++i;
            model->removeFilterId(id);
            continue;
        }
        if(model->contains(global))
            globalIndex = i;
        if(model->rowCount() == 0 && !model->all())
        {
            removeModelIndexes << i;
            listIds << model->filteredId();
        }
        ++i;
    }

    for(auto i : std::as_const(removeModelIndexes))
    {
        removeModel(i);
    }
    auto model = m_filterModels[0].get();

    std::for_each(std::begin(listIds), std::end(listIds), [model](const QString& id){
        model->removeFilterId(id);
    });

    if(globalIndex<0)
        return;

    if(model->rowCount() == 0)
    {
        auto ids = model->filteredId();
        if(ids.contains(global))
        {
            mergeGlobal(global, globalIndex);
        }
    }

}

void ChatroomSplitterModel::moveRight(const QString& id, int index)
{
    auto dest = modelFromIndex(index + 1);
    auto source = modelFromIndex(index);

    if(!dest || !source) {
        qCWarning(MessagingCat) << tr("[move right] Source or destination chatroom does not exist.");
        return;
    }

    if(source->all())
    {
        source->addFilterId(id);
    }
    else
    {
        source->removeFilterId(id);
    }

    dest->addFilterId(id);
}

void ChatroomSplitterModel::moveLeft(const QString& id, int index)
{
    auto dest = modelFromIndex(index - 1);
    auto source = modelFromIndex(index);

    if(!dest || !source) {
        qCWarning(MessagingCat) << tr("[move left] Source or destination chatroom does not exist.");
        return;
    }

    if(dest->all())
    {
        dest->removeFilterId(id);
    }
    else
    {
        dest->addFilterId(id);
    }

    source->removeFilterId(id);
}

FilterInstantMessagingModel* ChatroomSplitterModel::modelFromIndex(quint64 i)
{
    if(i < 0 || i >= m_filterModels.size())
        return nullptr;

    return m_filterModels[i].get();
}

} // namespace InstantMessaging
