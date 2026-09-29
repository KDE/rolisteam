/*************************************************************************
 *   Copyright (c) 2015 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   https://rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include "data/localpersonmodel.h"

#include "data/character.h"
#include "data/person.h"
#include "data/player.h"

#include <QDebug>

LocalPersonModel::LocalPersonModel() : QAbstractProxyModel() {}

QModelIndex LocalPersonModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    auto parent= sourceIndex.parent();

    if(!parent.isValid() && sourceIndex.row() > 0)
        return QModelIndex();

    if(!parent.isValid() && sourceIndex.row() == 0)
        return createIndex(0, 0, sourceIndex.internalPointer());

    if(parent.isValid() && parent.row() == 0)
        return createIndex(1 + sourceIndex.row(), 0, sourceIndex.internalPointer());

    return QModelIndex();
}

QModelIndex LocalPersonModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if(!sourceModel())
        return {};

    QModelIndex idx;
    if(proxyIndex.row() == 0)
        idx= sourceModel()->index(0, 0, QModelIndex());
    else
        idx= sourceModel()->index(proxyIndex.row() - 1, 0, sourceModel()->index(0, 0, QModelIndex()));

    return idx;
}

QHash<int, QByteArray> LocalPersonModel::roleNames() const
{
    return sourceModel()->roleNames();
}

void LocalPersonModel::setPlayerModel(PlayerModel* sourceModel)
{
    beginResetModel();
    QAbstractProxyModel::setSourceModel(sourceModel);
    endResetModel();

    auto func= [this]()
    {
        qDebug() << "[LOCALPERSONMODEL] func called";
        beginResetModel();

        endResetModel();
    };
    connect(sourceModel, &QAbstractItemModel::dataChanged, this, func);
    connect(sourceModel, &QAbstractItemModel::rowsInserted, this, func);
    connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, func);
    connect(sourceModel, &QAbstractItemModel::modelReset, this, func);
    connect(sourceModel, &PlayerModel::characterAdded, this, func);
    connect(sourceModel, &PlayerModel::characterAdded, this,
            [this, func](Player*, Character* c)
            {
                if(!c)
                    return;
                connect(c, &Character::nameChanged, this, func);
                connect(c, &Character::colorChanged, this, func);
                connect(c, &Character::avatarChanged, this, func);
            });
    connect(sourceModel, &PlayerModel::characterRemoved, this, func);
    connect(sourceModel, &PlayerModel::playerJoin, this,
            [this, func](Player* p)
            {
                if(!p)
                    return;
                connect(p, &Player::nameChanged, this, func);
                connect(p, &Player::colorChanged, this, func);
                connect(p, &Player::avatarChanged, this, func);

                auto ids= p->characterIds();
                for(auto const& id : std::as_const(ids))
                {
                    auto c= p->characterById(id);
                    if(!c)
                        continue;
                    connect(c, &Character::nameChanged, this, func);
                    connect(c, &Character::colorChanged, this, func);
                    connect(c, &Character::avatarChanged, this, func);
                }
            });

    auto p= sourceModel->localPlayerId();
    auto player= sourceModel->playerById(p);
    if(!player)
        return;
    connect(player, &Player::nameChanged, this, func);
    connect(player, &Player::colorChanged, this, func);
    connect(player, &Player::avatarChanged, this, func);

    auto ids= player->characterIds();
    for(auto const& id : std::as_const(ids))
    {
        auto c= player->characterById(id);
        if(!c)
            continue;
        connect(c, &Character::nameChanged, this, func);
        connect(c, &Character::colorChanged, this, func);
        connect(c, &Character::avatarChanged, this, func);
    }
}

QModelIndex LocalPersonModel::index(int r, int c, const QModelIndex&) const
{
    return createIndex(r, c);
}

QModelIndex LocalPersonModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int LocalPersonModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    auto firstItem= sourceModel()->index(0, 0);
    if(!firstItem.isValid())
        return 0;

    auto child= sourceModel()->rowCount(firstItem);

    return child + 1;
}

int LocalPersonModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return 1;
}
