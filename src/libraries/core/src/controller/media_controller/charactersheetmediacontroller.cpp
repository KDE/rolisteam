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
#include "charactersheetmediacontroller.h"

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "data/cleveruri.h"
#include "model/charactermodel.h"
#include "network/receiveevent.h"
#include "undoCmd/removemediacontrollercommand.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

#include <QDebug>

CharacterSheetController* findSheet(const std::vector<std::unique_ptr<CharacterSheetController>>& sheets,
                                    const QString& id)
{
    auto it= std::find_if(sheets.begin(), sheets.end(),
                          [id](const std::unique_ptr<CharacterSheetController>& ctrl) { return ctrl->uuid() == id; });
    if(sheets.end() == it)
        return nullptr;
    return (*it).get();
}

CharacterSheetMediaController::CharacterSheetMediaController(ContentModel* model, CharacterModel* characterModel)
    : MediaManagerBase(Core::ContentType::CHARACTERSHEET, model), m_characterModel(characterModel)
{
}

bool CharacterSheetMediaController::openMedia(const QString& id, const std::map<QString, QVariant>& args)
{
    if(id.isEmpty() && args.empty())
        return false;

    QHash<QString, QVariant> hash(args.begin(), args.end());
    hash.insert(QStringLiteral("id"), id);

    addCharacterSheet(hash);
    return true;
}

void CharacterSheetMediaController::closeMedia(const QString& id)
{
    /*  auto it= std::remove_if(m_sheets.begin(), m_sheets.end(),
                              [id](const std::unique_ptr<CharacterSheetController>& ctrl) { return ctrl->uuid() == id;
      }); if(it == m_sheets.end()) return;

      (*it)->aboutToClose();
      m_sheets.erase(it, m_sheets.end());*/
}

void CharacterSheetMediaController::registerNetworkReceiver()
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterCategory, this);
}

NetWorkReceiver::SendType CharacterSheetMediaController::processMessage(NetworkMessageReader* msg)
{
    auto type= NetWorkReceiver::NONE;
    if(msg->action() == NetMsg::addCharacterSheet && msg->category() == NetMsg::CharacterCategory)
    {
        auto hash= MessageHelper::readCharacterSheet(msg);
        addCharacterSheet(hash);
    }
    /*else if(msg->action() == NetMsg::updateFieldCharacterSheet && msg->category() == NetMsg::CharacterCategory)
    {
        auto find= msg->string8();
        auto current= findSheet(m_sheets, find);
        MessageHelper::readUpdateField(current, msg);
    }*/
    return type;
}

void CharacterSheetMediaController::addCharacterSheet(const QHash<QString, QVariant>& params)
{

    auto id= params.value(QStringLiteral("id")).toString();

    auto path= params.value(QStringLiteral("path")).toString();

    std::unique_ptr<CharacterSheetController> sheetCtrl(new CharacterSheetController(m_characterModel, id, path));

    sheetCtrl->setName(params.value(QStringLiteral("name")).toString());
    sheetCtrl->setQmlCode(params.value(QStringLiteral("qml")).toString());

    if(params.contains(QStringLiteral("imageData")))
    {
        auto array= params.value(QStringLiteral("imageData")).toByteArray();
        auto imgModel= sheetCtrl->imageModel();
        imgModel->load(IOHelper::byteArrayToJsonArray(array));
    }
    if(params.contains(QStringLiteral("rootSection")))
    {
        auto array= params.value(QStringLiteral("rootSection")).toByteArray();
        auto sheetModel= sheetCtrl->model();
        sheetModel->setRootSection(IOHelper::byteArrayToJsonObj(array));
    }

    connect(this, &CharacterSheetMediaController::localIsGMChanged, sheetCtrl.get(),
            &CharacterSheetController::setLocalGM);
    connect(this, &CharacterSheetMediaController::gameMasterIdChanged, sheetCtrl.get(),
            &CharacterSheetController::setGameMasterId);
    sheetCtrl->setGameMasterId(m_gameMasterId);

    // emit characterSheetCreated(sheetCtrl.get());

    if(params.contains(QStringLiteral("data")) && params.contains(QStringLiteral("characterId")))
    {
        auto array= params.value(QStringLiteral("data")).toByteArray();
        auto characterId= params.value(QStringLiteral("characterId")).toString();
        sheetCtrl->addCharacterSheet(IOHelper::byteArrayToJsonObj(array), characterId);
    }

    if(params.contains(QStringLiteral("serializedData")))
    {
        auto serializedData= params.value(QStringLiteral("serializedData")).toByteArray();
        IOHelper::readCharacterSheetController(sheetCtrl.get(), serializedData);
    }
    auto sheet= sheetCtrl.get();

    connect(sheetCtrl.get(), &CharacterSheetController::closeMe, this, [this, sheet]() {
        if(!m_undoStack)
            return;
        m_undoStack->push(new RemoveMediaControllerCommand(sheet, this));
    });
    // m_sheets.push_back(std::move(sheetCtrl));
}

QString CharacterSheetMediaController::gameMasterId() const
{
    return m_gameMasterId;
}

std::vector<CharacterSheetController*> CharacterSheetMediaController::controllers() const
{
    std::vector<CharacterSheetController*> vec;
    /*std::transform(m_sheets.begin(), m_sheets.end(), std::back_inserter(vec),
                   [](const std::unique_ptr<CharacterSheetController>& ctrl) { return ctrl.get(); });*/
    return vec;
}

void CharacterSheetMediaController::setGameMasterId(const QString& gameMasterId)
{
    if(m_gameMasterId == gameMasterId)
        return;

    m_gameMasterId= gameMasterId;
    emit gameMasterIdChanged(m_gameMasterId);
}
