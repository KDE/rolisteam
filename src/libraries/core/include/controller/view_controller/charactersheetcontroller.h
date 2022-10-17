/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   renaud@rolisteam.org                                     *
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
#ifndef CHARACTERSHEETCONTROLLER_H
#define CHARACTERSHEETCONTROLLER_H

#include <QAbstractItemModel>
#include <QJsonObject>
#include <QPointer>
#include <memory>
#include <set>

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "mediacontrollerbase.h"
#include "model/charactermodel.h"
#include "updater/media/charactersheetupdater.h"
#include <core_global.h>

class Player;
class Character;
class CharacterSheet;

struct CORE_EXPORT CharacterSheetData
{
    CharacterSheet* sheet;
    Player* player;
    Character* character;
};

class TreeSheetItem;
class CORE_EXPORT CharacterSheetController : public MediaControllerBase
{
    Q_OBJECT
    Q_PROPERTY(CharacterSheetModel* model READ model CONSTANT)
    Q_PROPERTY(charactersheet::ImageModel* imageModel READ imageModel CONSTANT)
    Q_PROPERTY(CharacterModel* characterModel READ characterModel CONSTANT)
    Q_PROPERTY(QString qmlCode READ qmlCode CONSTANT)
    Q_PROPERTY(bool cornerEnabled READ cornerEnabled NOTIFY cornerEnabledChanged)
    Q_PROPERTY(QString gameMasterId READ gameMasterId WRITE setGameMasterId NOTIFY gameMasterIdChanged)
    Q_PROPERTY(QJsonObject rootJson READ rootJson WRITE setRootJson NOTIFY rootJsonChanged)
public:
    CharacterSheetController(const QString& id= QString(), const QUrl& path= QUrl(), QObject* parent= nullptr);
    ~CharacterSheetController() override;

    CharacterSheetModel* model() const;
    charactersheet::ImageModel* imageModel() const;
    CharacterModel* characterModel() const;
    const QJsonObject& rootJson() const;

    QString qmlCode() const;
    QString gameMasterId() const;
    bool cornerEnabled() const;

    void updateFieldFrom(const QString& sheetId, const QJsonObject& obj, const QString& parentPath);

    static void setCharacterModel(CharacterModel* model);

    void setRootJson(const QJsonObject& newRootJson);

public slots:
    void shareCharacterSheetTo(const QString& uuid, int idx);
    void shareCharacterSheetToAll(int idx);
    void setQmlCode(const QString& qml);
    void addCharacterSheet(const QJsonObject& data, const QString& charId);
    void setGameMasterId(const QString& id);

signals:
    void cornerEnabledChanged(bool);
    void currentSheetChanged();
    void sheetCreated(CharacterSheet* sheet, Character* character);
    void gameMasterIdChanged();
    void rootJsonChanged();
    void share(CharacterSheetController* ctrl, CharacterSheet* sheet, CharacterSheetUpdater::SharingMode mode,
               Character* character, const QStringList& recipients);

private:
    std::unique_ptr<CharacterSheetModel> m_model;
    std::unique_ptr<charactersheet::ImageModel> m_imageModel;
    static QPointer<CharacterModel> m_characterModel;
    std::set<CharacterSheetData> m_sheetData;
    QJsonObject m_rootJson;
    QString m_qmlCode;
    QString m_gameMasterId;
};

#endif // CHARACTERSHEETCONTROLLER_H