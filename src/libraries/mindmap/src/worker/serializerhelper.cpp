#include "serializerhelper.h"

#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/minditem.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/data/packagenode.h"
#include "mindmap/model/minditemmodel.h"

#include <QJsonArray>
#include <QJsonObject>

namespace mindmap
{

namespace SerializerHelper
{

void updateMindItem(QJsonObject& obj, mindmap::MindItem* item)
{
    namespace ujm= mindmap;
    obj[ujm::JSON_MINDITEM_ID]= item->id();
    obj[ujm::JSON_MINDITEM_TEXT]= item->text();
    obj[ujm::JSON_MINDITEM_VISIBLE]= item->isVisible();
    obj[ujm::JSON_MINDITEM_SELECTED]= item->selected();
    obj[ujm::JSON_MINDITEM_TYPE]= item->type();
}

void updatePositionItem(QJsonObject& obj, mindmap::PositionedItem* pitem)
{
    namespace ujm= mindmap;
    updateMindItem(obj, pitem);
    obj[ujm::JSON_POSITIONED_POSITIONX]= pitem->position().x();
    obj[ujm::JSON_POSITIONED_POSITIONY]= pitem->position().y();
    obj[ujm::JSON_POSITIONED_CENTERX]= pitem->centerPoint().x();
    obj[ujm::JSON_POSITIONED_CENTERY]= pitem->centerPoint().y();
    obj[ujm::JSON_POSITIONED_WIDTH]= pitem->width();
    obj[ujm::JSON_POSITIONED_HEIGHT]= pitem->height();
    obj[ujm::JSON_POSITIONED_DRAGGED]= pitem->isDragged();
    obj[ujm::JSON_POSITIONED_OPEN]= pitem->open();
    obj[ujm::JSON_POSITIONED_LOCKED]= pitem->isLocked();
    obj[ujm::JSON_POSITIONED_MASS]= pitem->mass();
};

void readMindNode(QJsonObject& obj, mindmap::MindNode* node)
{
    if(!node)
        return;
    updatePositionItem(obj, node);
    obj[mindmap::JSON_NODE_STYLE]= node->styleIndex();
    obj[mindmap::JSON_NODE_DESC]= node->description();
    obj[mindmap::JSON_NODE_TAGS]= QJsonArray::fromStringList(node->tags());
}

void readPackageNode(QJsonObject& obj, mindmap::PackageNode* pack)
{
    if(!pack)
        return;
    updatePositionItem(obj, pack);
    obj[mindmap::JSON_PACK_TITLE]= pack->text();
    obj[mindmap::JSON_PACK_MINMARGE]= pack->minimumMargin();
    obj[mindmap::JSON_PACK_INTERNAL_CHILDREN]= QJsonArray::fromStringList(pack->childrenId());
}

void readLinkNode(QJsonObject& obj, mindmap::LinkController* link)
{
    if(!link)
        return;
    updateMindItem(obj, link);
    auto start= link->start();
    obj[mindmap::JSON_LINK_IDSTART]= start ? start->id() : QString();
    auto end= link->end();
    obj[mindmap::JSON_LINK_IDEND]= end ? end->id() : QString();
    obj[mindmap::JSON_LINK_DIRECTION]= static_cast<int>(link->direction());
}

void fetchMindItem(const QJsonObject& obj, mindmap::MindItem* item)
{
    namespace ujm= mindmap;
    item->setId(obj[ujm::JSON_MINDITEM_ID].toString());
    item->setText(obj[ujm::JSON_MINDITEM_TEXT].toString());
    item->setVisible(obj[ujm::JSON_MINDITEM_VISIBLE].toBool());
    item->setSelected(false); // obj[ujm::JSON_MINDITEM_SELECTED].toBool()
};

void fetchPositionItem(const QJsonObject& obj, mindmap::PositionedItem* pitem)
{
    namespace ujm= mindmap;
    mindmap::SerializerHelper::fetchMindItem(obj, pitem);
    pitem->setPosition(
        {obj[ujm::JSON_POSITIONED_POSITIONX].toDouble(), obj[ujm::JSON_POSITIONED_POSITIONY].toDouble()});

    qDebug() << "position" << pitem->position();

    pitem->setWidth(obj[ujm::JSON_POSITIONED_WIDTH].toDouble());
    pitem->setHeight(obj[ujm::JSON_POSITIONED_HEIGHT].toDouble());
    pitem->setDragged(obj[ujm::JSON_POSITIONED_DRAGGED].toBool());
    pitem->setOpen(obj[ujm::JSON_POSITIONED_OPEN].toBool());
    pitem->setLocked(obj[ujm::JSON_POSITIONED_LOCKED].toBool());
    pitem->setMass(obj[ujm::JSON_POSITIONED_MASS].toInt());
};

QJsonArray serializeMindmapNode(const std::vector<QPointer<mindmap::MindItem>>& items)
{
    QJsonArray res;
    for(auto const& item : items)
    {
        QJsonObject obj;
        switch(item->type())
        {
        case mindmap::MindItem::NodeType:
            readMindNode(obj, dynamic_cast<mindmap::MindNode*>(item.get()));
            break;
        case mindmap::MindItem::LinkType:
            readLinkNode(obj, dynamic_cast<mindmap::LinkController*>(item.get()));
            break;
        case mindmap::MindItem::PackageType:
            readPackageNode(obj, dynamic_cast<mindmap::PackageNode*>(item.get()));
            break;
        case mindmap::MindItem::InvalidType:
            qWarning() << "Invalid Type of Node";
            break;
        }
        res.append(obj);
    }
    return res;
}

mindmap::MindNode* fetchMindNode(const QJsonObject& obj)
{
    auto node= new mindmap::MindNode();
    fetchPositionItem(obj, node);

    node->setStyleIndex(obj[mindmap::JSON_NODE_STYLE].toInt());
    node->setDescription(obj[mindmap::JSON_NODE_DESC].toString());
    auto tagArray= obj[mindmap::JSON_NODE_TAGS].toArray();
    QStringList tags;
    std::transform(std::begin(tagArray), std::end(tagArray), std::back_inserter(tags),
                   [](const QJsonValue& val) { return val.toString(); });
    node->setTags(tags);
    return node;
}
mindmap::PackageNode* fetchPackage(const QJsonObject& pack, mindmap::MindItemModel* model)
{
    auto node= new mindmap::PackageNode();
    fetchPositionItem(pack, node);

    node->setText(pack[mindmap::JSON_PACK_TITLE].toString());
    node->setMinimumMargin(pack[mindmap::JSON_PACK_MINMARGE].toInt());

    auto childArray= pack[mindmap::JSON_PACK_INTERNAL_CHILDREN].toArray();
    std::for_each(std::begin(childArray), std::end(childArray),
                  [node, model](const QJsonValue& val) { node->addChild(model->positionItem(val.toString())); });

    return node;
}
mindmap::LinkController* fetchLink(const QJsonObject& obj, mindmap::MindItemModel* model)
{
    auto link= new mindmap::LinkController();
    fetchMindItem(obj, link);

    auto startId= obj[mindmap::JSON_LINK_IDSTART].toString();
    auto endId= obj[mindmap::JSON_LINK_IDEND].toString();
    auto start= model->positionItem(startId);
    auto end= model->positionItem(endId);

    link->setStart(start);
    link->setEnd(end);
    link->setDirection(static_cast<mindmap::LinkController::Direction>(obj[mindmap::JSON_LINK_DIRECTION].toInt()));
    return link;
}

QList<mindmap::MindItem*> readMindmapNode(QJsonArray& array, mindmap::MindItemModel* model)
{
    QList<mindmap::MindItem*> res;

    for(auto val : array)
    {
        auto obj= val.toObject();
        auto type= static_cast<mindmap::MindItem::Type>(obj[mindmap::JSON_MINDITEM_TYPE].toInt());
        mindmap::MindItem* item= nullptr;
        switch(type)
        {
        case mindmap::MindItem::NodeType:
            item= fetchMindNode(obj);
            break;
        case mindmap::MindItem::LinkType:
            item= fetchLink(obj, model);
            break;
        case mindmap::MindItem::PackageType:
            item= fetchPackage(obj, model);
            break;
        case mindmap::MindItem::InvalidType:
            qWarning() << "Invalid Type of Node";
            break;
        }
        if(!item)
            continue;
        res.append(item);
    }
    return res;
}
} // namespace SerializerHelper
} // namespace mindmap
