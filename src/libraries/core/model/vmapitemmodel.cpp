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
#include "vmapitemmodel.h"

#include <set>

namespace vmap
{
VmapItemModel::VmapItemModel(QObject* parent) : QAbstractListModel(parent) {}

VmapItemModel::~VmapItemModel()= default;

int VmapItemModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return 0;

    return static_cast<int>(m_items.size());
}

QVariant VmapItemModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    std::set<int> allowedRoles({Qt::DisplayRole, IdRole, SelectedRole, EditableRole, SelectableRole, VisibleRole,
                                OpacityRole, RotationRole, LayerRole, PositionRole, LocalGmRole, ColorRole, LockedRole,
                                ToolRole, TypeRole, InitializedRole, ControllerRole});

    if(allowedRoles.find(role) == allowedRoles.end())
        return QVariant();

    auto item= m_items.at(static_cast<std::size_t>(index.row())).get();

    if(nullptr == item)
        return {};

    QVariant var;
    switch(role)
    {
    case Qt::DisplayRole:
    case IdRole:
        var= item->uuid();
        break;
    case SelectedRole:
        var= item->selected();
        break;
    case EditableRole:
        var= item->editable();
        break;
    case SelectableRole:
        var= item->selectable();
        break;
    case VisibleRole:
        var= item->visible();
        break;
    case OpacityRole:
        var= item->opacity();
        break;
    case RotationRole:
        var= item->rotation();
        break;
    case LayerRole:
        var= QVariant::fromValue(item->layer());
        break;
    case PositionRole:
        var= item->pos();
        break;
    case LocalGmRole:
        var= item->localIsGM();
        break;
    case ColorRole:
        var= item->color();
        break;
    case LockedRole:
        var= item->locked();
        break;
    case ToolRole:
        var= item->tool();
        break;
    case TypeRole:
        var= item->itemType();
        break;
    case InitializedRole:
        var= item->initialized();
        break;
    case ControllerRole:
        var= QVariant::fromValue(item);
        break;
    }

    return var;
}

bool vmap::VmapItemModel::appendItemController(vmap::VisualItemController* item)
{
    auto size= static_cast<int>(m_items.size());
    std::unique_ptr<vmap::VisualItemController> ctrl(item);
    beginInsertRows(QModelIndex(), size, size);
    m_items.push_back(std::move(ctrl));
    endInsertRows();

    emit itemControllerAdded(item);
    return true;
}

bool vmap::VmapItemModel::removeItemController(const QString& uuid)
{
    auto it= std::find_if(
        std::begin(m_items), std::end(m_items),
        [uuid](const std::unique_ptr<vmap::VisualItemController>& itemCtrl) { return uuid == itemCtrl->uuid(); });

    if(it == std::end(m_items))
        return false;

    /* (*it)->aboutToClose();
     if((*it)->localIsOwner())
         MessageHelper::closeMedia(uuid, (*it)->contentType());*/

    auto pos= static_cast<int>(std::distance(std::begin(m_items), it));

    beginRemoveRows(QModelIndex(), pos, pos);
    m_items.erase(it);
    endRemoveRows();

    return true;
}

void vmap::VmapItemModel::clearData()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

std::vector<VisualItemController*> VmapItemModel::items() const
{
    std::vector<VisualItemController*> vec;
    vec.reserve(m_items.size());

    std::transform(std::begin(m_items), std::end(m_items), std::back_inserter(vec),
                   [](const std::unique_ptr<vmap::VisualItemController>& item) { return item.get(); });

    return vec;
}

VisualItemController* VmapItemModel::item(const QString& id) const
{
    auto it= std::find_if(
        std::begin(m_items), std::end(m_items),
        [id](const std::unique_ptr<vmap::VisualItemController>& itemCtrl) { return id == itemCtrl->uuid(); });

    if(it == std::end(m_items))
        return nullptr;

    return it->get();
}
} // namespace vmap
