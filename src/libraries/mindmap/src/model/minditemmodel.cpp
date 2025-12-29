/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#include "minditemmodel.h"

#include <QColor>
#include <QDebug>
#include <QRectF>

#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/packagenode.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/nodeimageprovider.h"

namespace mindmap
{
std::tuple<std::vector<std::unique_ptr<MindItem>>&, int> getVector(std::vector<std::unique_ptr<MindItem>>& links,
                                                                   std::vector<std::unique_ptr<MindItem>>& package,
                                                                   std::vector<std::unique_ptr<MindItem>>& node,
                                                                   MindItem::Type type)
{

    if(type == MindItem::LinkType)
        return {links, 0};
    else if(type == MindItem::PackageType)
        return {package, links.size()};
    else // if(type == MindItem::NodeType)
        return {node, links.size() + package.size()};
}

MindItem* itemFromIndex(int r, const std::vector<std::unique_ptr<MindItem>>& links,
                        const std::vector<std::unique_ptr<MindItem>>& packages,
                        const std::vector<std::unique_ptr<MindItem>>& nodes)
{
    auto linkCount= static_cast<int>(links.size());
    auto packageCount= static_cast<int>(packages.size());
    auto nodeCount= static_cast<int>(nodes.size());

    MindItem* mindNode= nullptr;

    if(r < linkCount)
    {
        mindNode= links[r].get();
    }
    else
    {
        r-= linkCount;
        if(r < packageCount)
            mindNode= packages[r].get();
        else
        {
            r-= packageCount;
            if(r < nodeCount)
                mindNode= nodes[r].get();
        }
    }
    return mindNode;
}

MindItemModel::MindItemModel(ImageModel* imgModel, QObject* parent) : QAbstractListModel(parent), m_imgModel(imgModel)
{
}

MindItemModel::~MindItemModel() {}

int MindItemModel::rowCount(const QModelIndex& parent) const
{
    // qDebug() << "rowCount :";
    if(parent.isValid())
        return 0;
    // qDebug() << "links" << m_links.size() << "node:" << m_nodes.size();
    return static_cast<int>(m_links.size() + m_packages.size() + m_nodes.size());
}

QVariant MindItemModel::data(const QModelIndex& index, int role) const
{

    if(!index.isValid())
        return QVariant();

    QVariant result;

    auto r= index.row();
    auto mindNode= itemFromIndex(r, m_links, m_packages, m_nodes);

    if(!mindNode)
        return {};

    if(role == Qt::DisplayRole)
        role= Label;

    QSet<int> allowedRole{Visible, Label, Selected, Type, Uuid, Object, HasPicture};

    if(!allowedRole.contains(role))
        return {};

    switch(role)
    {
    case Visible:
        result= mindNode->isVisible();
        break;
    case Label:
        result= mindNode->text().isEmpty() ? QString("Node") : mindNode->text();
        break;
    case Selected:
        result= mindNode->selected();
        break;
    case Type:
        result= mindNode->type();
        break;
    case Uuid:
        result= mindNode->id();
        break;
    case Object:
        result= QVariant::fromValue(mindNode);
        break;
    case HasPicture:
        result= mindNode->type() == MindItem::NodeType ? m_imgModel->hasPixmap(mindNode->id()) : false;
        break;
    }
    return result;
}

bool MindItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(data(index, role) != value)
    {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

void MindItemModel::update(const QString& id, int role)
{
    auto current= item(id);

    if(!current)
        return;

    int row= 0;
    auto findFunc= [current](const std::unique_ptr<MindItem>& item) { return current == item.get(); };
    if(current->type() == MindItem::LinkType)
    {
        auto it= std::find_if(std::begin(m_links), std::end(m_links), findFunc);
        if(it != std::end(m_links))
            row= std::distance(std::begin(m_links), it);
    }
    else if(current->type() == MindItem::PackageType)
    {
        auto it= std::find_if(std::begin(m_packages), std::end(m_packages), findFunc);
        if(it != std::end(m_packages))
            row= std::distance(std::begin(m_packages), it) + m_links.size();
    }
    else if(current->type() == MindItem::NodeType)
    {
        auto it= std::find_if(std::begin(m_nodes), std::end(m_nodes), findFunc);
        if(it != std::end(m_nodes))
            row= std::distance(std::begin(m_nodes), it) + m_links.size() + m_packages.size();
    }

    emit dataChanged(index(row, 0), index(row, 0), {role});
}

void MindItemModel::setImageUriToNode(const QString& id)
{
    auto it= std::find_if(m_nodes.begin(), m_nodes.end(),
                          [id](const std::unique_ptr<MindItem>& node) { return node->id() == id; });
    if(it == m_nodes.end())
        return;

    auto dis= std::distance(m_nodes.begin(), it);
    auto node= dynamic_cast<MindNode*>(it->get());
    if(!node)
        return;

    auto idx= index(dis, 0, QModelIndex());
    emit dataChanged(idx, idx, QVector<int>());
}

Qt::ItemFlags MindItemModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QHash<int, QByteArray> MindItemModel::roleNames() const
{
    static QHash<int, QByteArray> roles= {{MindItemModel::Label, "label"},
                                          {MindItemModel::Visible, "isVisible"},
                                          {MindItemModel::Selected, "isSelected"},
                                          {MindItemModel::Type, "type"},
                                          {MindItemModel::Uuid, "id"},
                                          {MindItemModel::Object, "objectItem"},
                                          {MindItemModel::HasPicture, "hasPicture"}};
    return roles;
}

void MindItemModel::clear()
{
    beginResetModel();
    m_links.clear();
    m_packages.clear();
    m_nodes.clear();
    endResetModel();
}

bool MindItemModel::isPackageChild(const QString& id)
{
    for(auto const& item : m_packages)
    {
        if(!item.get())
            continue;

        auto package= dynamic_cast<mindmap::PackageNode*>(item.get());
        if(!package)
            continue;

        if(package->containsChild(id))
            return true;
    }
    return false;
}

void MindItemModel::removeItemFromPackage(const QString& id, bool network)
{
    for(auto const& item : m_packages)
    {
        if(!item.get())
            continue;

        auto package= dynamic_cast<mindmap::PackageNode*>(item.get());
        if(!package)
            continue;

        package->removeChild(id, network);
    }
}

void MindItemModel::appendItem(const QList<MindItem*>& nodes, bool network)
{
    for(auto const& node : nodes)
    {
        if(node == nullptr)
            return;

        auto [vec, offset]= getVector(m_links, m_packages, m_nodes, node->type());

        // prevent adding twice the same
        auto it= std::find_if(std::begin(vec), std::end(vec),
                              [node](const std::unique_ptr<MindItem>& item) { return item->id() == node->id(); });
        if(it != std::end(vec))
            continue;

        int row= offset + vec.size();

        if(node->type() == MindItem::LinkType)
        {
            auto link= dynamic_cast<mindmap::LinkController*>(node);
            if(link)
            {
                connect(link, &mindmap::LinkController::startPointChanged, this,
                        [this, link]()
                        {
                            QModelIndex parent;
                            auto it= std::find_if(m_links.begin(), m_links.end(),
                                                  [link](const std::unique_ptr<MindItem>& item)
                                                  { return item->id() == link->id(); });
                            if(it == m_links.end())
                                return;
                            auto offset= std::distance(m_links.begin(), it);
                            auto idx1= index(offset, 0, parent);
                            auto start= link->start();
                            emit dataChanged(idx1, idx1,
                                             start->isDragged() ? QVector<int>{Object, LinkStartPosition} :
                                                                  QVector<int>{Object, LinkPositionFromSpacing});
                        });
            }
        }
        else
        {
            auto pItem= dynamic_cast<mindmap::PositionedItem*>(node);
            if(pItem)
            {
                connect(pItem, &mindmap::PositionedItem::positionChanged, this, &MindItemModel::geometryChanged);
                connect(pItem, &mindmap::PositionedItem::textChanged, this, &MindItemModel::geometryChanged);
                connect(pItem, &mindmap::PositionedItem::textChanged, this,
                        [pItem, this]()
                        {
                            auto [vec, offset]= getVector(m_links, m_packages, m_nodes, pItem->type());
                            auto row= offset + static_cast<int>(vec.size());
                            auto idx= index(row, 0);
                            emit dataChanged(idx, idx, {Roles::Label});
                        });
                connect(pItem, &mindmap::PositionedItem::widthChanged, this, &MindItemModel::geometryChanged);
                connect(pItem, &mindmap::PositionedItem::heightChanged, this, &MindItemModel::geometryChanged);
            }
        }

        beginInsertRows(QModelIndex(), row, row);
        std::unique_ptr<mindmap::MindItem> uni(node);
        vec.push_back(std::move(uni));
        endInsertRows();
    }
    if(!network)
        emit itemAdded(nodes);
}
std::vector<PositionedItem*> MindItemModel::positionnedItems() const
{
    std::vector<PositionedItem*> vec;
    vec.reserve(m_packages.size() + m_nodes.size());

    for(auto const& item : m_packages)
    {
        auto pack= dynamic_cast<PositionedItem*>(item.get());

        if(!pack)
            continue;

        vec.push_back(pack);
    }

    for(auto const& item : m_nodes)
    {
        auto pack= dynamic_cast<PositionedItem*>(item.get());

        if(!pack)
            continue;

        vec.push_back(pack);
    }

    return vec;
}

MindItem* MindItemModel::createItem(MindItem::Type type)
{
    MindItem* result= nullptr;

    switch(type)
    {
    case MindItem::NodeType:
    {
        auto root= new MindNode();
        root->setStyleIndex(defaultStyleIndex());
        result= root;
    }
    break;
    case MindItem::PackageType:
    {
        auto pack= new PackageNode();
        emit latestInsertedPackage(pack);
        result= pack;
    }
    break;
    case MindItem::LinkType:
    {
        result= new LinkController();
    }
    break;
    case mindmap::MindItem::InvalidType:
        break;
    }

    return result;
}

void MindItemModel::removeAllSubItem(const mindmap::PositionedItem* item, QSet<QString>& items)
{
    QList<const mindmap::MindItem*> links{};

    /*[current](const std::unique_ptr<MindItem>& item){
        return current == item.get();
    }*/

    std::for_each(std::begin(m_links), std::end(m_links),
                  [&links, item, this](const std::unique_ptr<MindItem>& tmp)
                  {
                      auto const link= dynamic_cast<const mindmap::LinkController*>(tmp.get());
                      Q_ASSERT(link);
                      if(link->relatedTo(item->id()))
                      {
                          disconnect(link, 0, this, 0);
                          links << link;
                      }
                  });

    std::for_each(std::begin(links), std::end(links),
                  [this, &items](const mindmap::MindItem* tmp)
                  {
                      items << tmp->id();
                      auto it= std::find_if(std::begin(m_links), std::end(m_links),
                                            [tmp](const std::unique_ptr<mindmap::MindItem>& node)
                                            { return node.get() == tmp; });

                      if(it == std::end(m_links))
                          return;

                      auto idx= std::distance(std::begin(m_links), it);
                      beginRemoveRows(QModelIndex(), idx, idx);
                      m_links.erase(it);
                      endRemoveRows();
                  });

    std::for_each(std::begin(m_nodes), std::end(m_nodes),
                  [item](const std::unique_ptr<mindmap::MindItem>& tmp)
                  {
                      auto pItem= dynamic_cast<PositionedItem*>(tmp.get());

                      if(!pItem)
                          return;

                      if(pItem->parentId() == item->id())
                      {
                          pItem->setParentNode(nullptr); // remove parent
                      }
                  });
}

bool MindItemModel::removeItem(const MindItem* item)
{
    if(item == nullptr)
        return false;

    QSet<QString> items{item->id()};
    auto [vec, offset]= getVector(m_links, m_packages, m_nodes, item->type());

    auto it= std::find_if(vec.begin(), vec.end(),
                          [item](const std::unique_ptr<MindItem>& node) { return item == node.get(); });

    if(it == std::end(m_nodes))
        return false;

    auto idx= offset + static_cast<int>(std::distance(vec.begin(), it));

    if(item->type() == MindItem::LinkType)
    {
        auto const link= dynamic_cast<const mindmap::LinkController*>(item);
        if(link)
        {
            disconnect(link, 0, this, 0);
        }
    }
    else if(item->type() == MindItem::NodeType)
    {
        // removeAllSubItem(dynamic_cast<const mindmap::PositionedItem*>(item), items);
    }
    else if(item->type() == MindItem::PackageType)
    {
        removeAllSubItem(dynamic_cast<const mindmap::PositionedItem*>(item), items);
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    vec.erase(it);
    endRemoveRows();

    emit itemRemoved(items.values());

    return true;
}

void MindItemModel::openItem(const QString& id, bool status)
{
    auto it= item(id);

    if(nullptr == it)
        return;

    auto node= dynamic_cast<PositionedItem*>(it);

    if(nullptr == node)
        return;

    if(node->open() == status)
        return;

    node->setOpen(status);
}

MindItem* MindItemModel::item(const QString& id) const
{
    // MindItem* result= nullptr;

    auto it= std::find_if(m_nodes.begin(), m_nodes.end(),
                          [id](const std::unique_ptr<MindItem>& node) { return node->id() == id; });

    if(it != m_nodes.end())
        return it->get();

    auto it2= std::find_if(m_links.begin(), m_links.end(),
                           [id](const std::unique_ptr<MindItem>& node) { return node->id() == id; });

    if(it2 != m_links.end())
        return it2->get();

    auto it3= std::find_if(m_packages.begin(), m_packages.end(),
                           [id](const std::unique_ptr<MindItem>& node) { return node->id() == id; });

    if(it3 != m_packages.end())
        return it3->get();

    return nullptr;
}

PositionedItem* MindItemModel::positionItem(const QString& id) const
{
    auto it= std::find_if(m_nodes.begin(), m_nodes.end(),
                          [id](const std::unique_ptr<MindItem>& node) { return node->id() == id; });

    if(it != m_nodes.end())
        return dynamic_cast<PositionedItem*>(it->get());

    auto it3= std::find_if(m_packages.begin(), m_packages.end(),
                           [id](const std::unique_ptr<MindItem>& node) { return node->id() == id; });

    if(it3 != m_packages.end())
        return dynamic_cast<PositionedItem*>(it3->get());

    return nullptr;
}

QRectF MindItemModel::contentRect() const
{
    QRectF rect(0., 0., 1., 1.);

    for(auto const& item : m_nodes)
    {
        auto node= dynamic_cast<PositionedItem*>(item.get());
        if(!node)
            continue;
        rect= node->boundingRect().united(rect);
    }

    for(auto const& item : m_packages)
    {
        auto pack= dynamic_cast<PositionedItem*>(item.get());
        if(!pack)
            continue;
        rect= pack->boundingRect().united(rect);
    }
    return rect;
}

std::vector<std::unique_ptr<mindmap::MindItem>>& MindItemModel::items(MindItem::Type type)
{
    return std::get<0>(getVector(m_links, m_packages, m_nodes, type));
}

std::vector<LinkController*> MindItemModel::sublink(const QString& id) const
{
    std::vector<LinkController*> vec;

    for(auto const& item : m_links)
    {
        auto link= dynamic_cast<LinkController*>(item.get());
        if(!link)
            continue;

        auto st= link->start();
        if(!st)
            continue;

        if(id == st->id() && (std::end(vec) == std::find(std::begin(vec), std::end(vec), link)))
            vec.push_back(link);

        auto end= link->end();
        if(!end)
            continue;

        if(id == end->id() && (std::end(vec) == std::find(std::begin(vec), std::end(vec), link)))
            vec.push_back(link);
    }

    return vec;
}

QString MindItemModel::idFromIndex(int index) const
{
    auto mindNode= itemFromIndex(index, m_links, m_packages, m_nodes);

    if(!mindNode)
        return {};

    return mindNode->id();
}

int MindItemModel::defaultStyleIndex() const
{
    return m_defaultStyleIndex;
}

void MindItemModel::setDefaultStyleIndex(int newDefaultStyleIndex)
{
    if(m_defaultStyleIndex == newDefaultStyleIndex)
        return;
    m_defaultStyleIndex= newDefaultStyleIndex;
    emit defaultStyleIndexChanged();
}

PositionedItem* MindItemModel::parentNode(const QString& id)
{
    PositionedItem* res= nullptr;
    auto it= std::find_if(std::begin(m_links), std::end(m_links),
                          [id](const std::unique_ptr<MindItem>& item)
                          {
                              auto link= dynamic_cast<LinkController*>(item.get());
                              if(!link)
                                  return false;
                              auto endPoint= link->end();
                              if(!endPoint)
                                  return false;

                              return endPoint->id() == id;
                          });

    if(it != std::end(m_links))
    {
        auto found= dynamic_cast<LinkController*>(it->get());
        if(found)
            res= found->start();
    }

    return res;
}

} // namespace mindmap
