#ifndef MINDMAP_SERIALIZERHELPER_H
#define MINDMAP_SERIALIZERHELPER_H

#include "mindmap/mindmap_global.h"

#include <QJsonObject>
#include <QString>

namespace mindmap
{
constexpr auto JSON_CTRL_DEFAULT_INDEX_STYLE{"defaultIndexStyle"};
constexpr auto JSON_CTRL_SPACING{"spacing"};
constexpr auto JSON_CTRL_LINK_LABEL_VISIBILITY{"linklabelvisibility"};

constexpr auto JSON_MINDITEM_ID{"id"};
constexpr auto JSON_MINDITEM_TEXT{"text"};
constexpr auto JSON_MINDITEM_VISIBLE{"visible"};
constexpr auto JSON_MINDITEM_SELECTED{"selected"};
constexpr auto JSON_MINDITEM_TYPE{"type"};

constexpr auto JSON_POSITIONED_POSITIONX{"x"};
constexpr auto JSON_POSITIONED_POSITIONY{"y"};
constexpr auto JSON_POSITIONED_CENTERX{"center.x"};
constexpr auto JSON_POSITIONED_CENTERY{"center.y"};
constexpr auto JSON_POSITIONED_WIDTH{"width"};
constexpr auto JSON_POSITIONED_HEIGHT{"height"};
constexpr auto JSON_POSITIONED_DRAGGED{"isDragged"};
constexpr auto JSON_POSITIONED_OPEN{"open"};
constexpr auto JSON_POSITIONED_LOCKED{"locked"};
constexpr auto JSON_POSITIONED_MASS{"mass"};

constexpr auto JSON_NODE_IMAGE{"image"};
constexpr auto JSON_NODE_STYLE{"styleindex"};
constexpr auto JSON_NODE_DESC{"description"};
constexpr auto JSON_NODE_TAGS{"tags"};
constexpr auto JSON_NODES{"nodes"};

constexpr auto JSON_LINKS{"links"};
constexpr auto JSON_LINK_IDSTART{"idStart"};
constexpr auto JSON_LINK_IDEND{"idEnd"};
constexpr auto JSON_LINK_DIRECTION{"direction"};

constexpr auto JSON_PACK_TITLE{"title"};
constexpr auto JSON_PACK_MINMARGE{"minimumMargin"};
constexpr auto JSON_PACK_INTERNAL_CHILDREN{"internalChildren"};
constexpr auto JSON_PACK_PACKAGES{"packages"};

constexpr auto JSON_IMGS{"images"};
constexpr auto JSON_IMG_ID{"id"};
constexpr auto JSON_IMG_DATA{"data"};
constexpr auto JSON_IMG_URL{"url"};

class PositionedItem;
class LinkController;
class PackageNode;
class MindNode;
class MindItem;
class MindItemModel;
namespace SerializerHelper
{
// Data to Json
MINDMAP_EXPORT void updatePositionItem(QJsonObject& obj, mindmap::PositionedItem* pitem);
MINDMAP_EXPORT void readLinkNode(QJsonObject& obj, mindmap::LinkController* link);
MINDMAP_EXPORT void readPackageNode(QJsonObject& obj, mindmap::PackageNode* pack);
MINDMAP_EXPORT void readMindNode(QJsonObject& obj, mindmap::MindNode* node);
MINDMAP_EXPORT QJsonArray serializeMindmapNode(const std::vector<QPointer<mindmap::MindItem>>& items);

MINDMAP_EXPORT void fetchPositionItem(const QJsonObject& obj, mindmap::PositionedItem* pitem);
MINDMAP_EXPORT void fetchMindItem(const QJsonObject& obj, mindmap::MindItem* item);
MINDMAP_EXPORT mindmap::MindNode* fetchMindNode(const QJsonObject& obj);
MINDMAP_EXPORT mindmap::PackageNode* fetchPackage(const QJsonObject& obj, mindmap::MindItemModel* model);
MINDMAP_EXPORT mindmap::LinkController* fetchLink(const QJsonObject& obj, mindmap::MindItemModel* model);
MINDMAP_EXPORT QList<mindmap::MindItem*> readMindmapNode(QJsonArray& array, MindItemModel* model);
} // namespace SerializerHelper
} // namespace mindmap
#endif // MINDMAP_SERIALIZERHELPER_H
