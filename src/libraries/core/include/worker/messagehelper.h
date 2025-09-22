/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <QHash>
#include <QPixmap>
#include <QString>
#include <QVariant>

#include "media/mediatype.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/minditemmodel.h"
#include "network/networkmessage.h"
#include "updater/media/charactersheetupdater.h"
#include <core_global.h>

namespace vmap
{
class RectController;
class TextController;
class LineController;
class PathController;
class ImageItemController;
class CharacterItemController;
class EllipseController;
class VisualItemController;
} // namespace vmap

namespace mindmap
{
class MindItem;
} // namespace mindmap

class DiceAliasModel;
class DiceAlias;
class CharacterStateModel;
class CharacterState;
class MediaControllerBase;
class ImageController;
class NetworkMessageWriter;
class NetworkMessageReader;
class PlayerModel;
class WebpageController;
class PdfController;
class VectorialMapController;
class Character;
class CharacterSheet;
class CharacterSheetController;
class SharedNoteController;
class MindMapController;
class CORE_EXPORT MessageHelper
{
public:
    static void sendOffGoodBye();
    static void closeMedia(const QString& id, Core::ContentType type);

    static void sendOffAllDiceAlias(DiceAliasModel* model);
    static void sendOffDiceAliasRemoved(int i);
    static void sendOffDiceAliasMoved(int i, int j);
    static void sendOffOneDiceAlias(const DiceAlias* da, int row);

    static void sendOffAllCharacterState(CharacterStateModel* model);
    static void sendOffOneCharacterState(const CharacterState* da, int row);
    static void sendOffCharacterStateRemoved(const QString& id);
    static void sendOffCharacterStateMoved(int i, int j);

    static QString readPlayerId(NetworkMessageReader& msg);

    // vmap
    static QHash<QString, QVariant> readVectorialMapData(NetworkMessageReader* msg);
    static void sendOffVMap(VectorialMapController* ctrl);

    static const std::map<QString, QVariant> readRect(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readLine(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readEllipse(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readImage(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readText(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readPath(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readCharacter(NetworkMessageReader* msg);

    static void sendOffRect(const vmap::RectController* ctrl, const QString& mapId);
    static void sendOffLine(const vmap::LineController* ctrl, const QString& mapId);
    static void sendOffEllispe(const vmap::EllipseController* ctrl, const QString& mapId);
    static void sendOffText(const vmap::TextController* ctrl, const QString& mapId);
    static void sendOffPath(const vmap::PathController* ctrl, const QString& mapId);
    static void sendOffImage(const vmap::ImageItemController* ctrl, const QString& mapId);
    static void sendOffCharacter(const vmap::CharacterItemController* ctrl, const QString& mapId);

    // media
    static QString readMediaId(NetworkMessageReader* msg);
    static QHash<QString, QVariant> readMediaData(NetworkMessageReader* msg);
    static void sendOffMediaControllerBase(const MediaControllerBase* ctrl, NetworkMessageWriter& msg);

    // image
    static void sendOffImage(ImageController* ctrl);
    static QHash<QString, QVariant> readImageData(NetworkMessageReader* msg);

    // PDF
    static void sendOffPdfFile(PdfController* ctrl);
    static QHash<QString, QVariant> readPdfData(NetworkMessageReader* msg);

    // webpage
    static void shareWebpage(WebpageController* ctrl);
    static void updateWebpage(WebpageController* ctrl);
    static QHash<QString, QVariant> readWebPageData(NetworkMessageReader* msg);
    static void readUpdateWebpage(WebpageController* ctrl, NetworkMessageReader* msg);

    // charactersheet
    static void stopSharingSheet(const QString& sheetId, const QString& ctrlId, const QString& characterId);
    static void shareCharacterSheet(CharacterSheet* sheet, Character* character, CharacterSheetController* ctrl,
                                    CharacterSheetUpdater::SharingMode mode);
    static QHash<QString, QVariant> readCharacterSheet(NetworkMessageReader* msg);
    static void readUpdateField(CharacterSheetController* ctrl, NetworkMessageReader* msg);

    // sharedNotes
    static void shareNotesTo(const SharedNoteController* ctrl, const QStringList& recipiants);
    static QHash<QString, QVariant> readSharedNoteData(NetworkMessageReader* msg);
    static void closeNoteTo(SharedNoteController* sharedCtrl, const QString& id);

    // Mindmap
    static void sendOffMindmapToAll(MindMapController* ctrl);
    static void sendOffMindmapPermissionUpdate(MindMapController* ctrl);
    static void openMindmapTo(MindMapController* ctrl, const QString& id);
    static void closeMindmapTo(MindMapController* ctrl, const QString& id);
    static void sendOffMindmapPermissionUpdateTo(Core::SharingPermission perm, MindMapController* ctrl,
                                                 const QString& id);
    static QHash<QString, QVariant> readMindMap(NetworkMessageReader* msg);
    static void readMindMapAddItem(MindMapController* ctrl, NetworkMessageReader* msg);
    static void buildAddItemMessage(NetworkMessageWriter& msg, const QList<mindmap::MindItem*>& nodes);
    static void buildRemoveItemMessage(NetworkMessageWriter& msg, const QStringList& nodes, const QStringList& links);
    static void readMindMapRemoveMessage(MindMapController* ctrl, NetworkMessageReader* msg);
    static void readChildPackageAction(bool add, NetworkMessageReader* msg, MindMapController* ctrl);

    // data
    static void updatePerson(NetworkMessageReader& data, PlayerModel* playerModel);
    static QStringList readIdList(NetworkMessageReader& data);
    static void fetchCharacterStatesFromNetwork(NetworkMessageReader* data, CharacterStateModel* model);
    static void fetchDiceAliasFromNetwork(NetworkMessageReader* data, QList<DiceAlias*>* list);

    // ImageModel
    static void sendOffImageInfo(const mindmap::ImageInfo& info, MediaControllerBase* ctrl);
    static void sendOffRemoveImageInfo(const QString& id, MediaControllerBase* ctrl);
    static void readAddSubImage(mindmap::ImageModel* model, mindmap::MindItemModel* items, NetworkMessageReader* msg);
    static void readRemoveSubImage(mindmap::ImageModel* model, NetworkMessageReader* msg);

    // MusicPlayer
    static void sendOffPlaySong(const QString& songName, qint64 time, int player);
    static void sendOffMusicPlayerOrder(NetMsg::Action netAction, int player);
    static void sendOffTime(qint64 time, int player);
    static void sendOffStopPlaying(int player);

    static void convertVisualItemCtrlAndAdd(vmap::VisualItemController* ctrl, NetworkMessageWriter& msg);

    static void addCharacterController(const vmap::CharacterItemController* ctrl, NetworkMessageWriter& msg);
    static void addPathController(const vmap::PathController* ctrl, NetworkMessageWriter& msg);
    static void addTextController(const vmap::TextController* ctrl, NetworkMessageWriter& msg);
    static void addImageController(const vmap::ImageItemController* ctrl, NetworkMessageWriter& msg);
    static void addLineController(const vmap::LineController* ctrl, NetworkMessageWriter& msg);
    static void addEllipseController(const vmap::EllipseController* ctrl, NetworkMessageWriter& msg);
};

#endif // MESSAGEHELPER_H
