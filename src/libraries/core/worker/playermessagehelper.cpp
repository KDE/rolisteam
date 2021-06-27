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
#include "playermessagehelper.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>

#include "data/character.h"
#include "data/player.h"
#include "network/clientmanager.h"
#include "userlist/playermodel.h"

PlayerMessageHelper::PlayerMessageHelper() {}

void PlayerMessageHelper::sendOffConnectionInfo(Player* player, const QByteArray& password)
{
    if(player == nullptr)
        return;

    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ConnectionInfo);
    msg.byteArray32(password);
    msg.string32(player->name());
    msg.string32(player->uuid());
    msg.sendToServer();
    // setLocalFeatures(*player);
    // writePlayerIntoMessage(msg, player);
}

void PlayerMessageHelper::sendOffPlayerInformations(Player* player)
{
    NetworkMessageWriter message(NetMsg::PlayerCategory, NetMsg::PlayerConnectionAction);
    // setLocalFeatures(*player);
    writePlayerIntoMessage(message, player);
    message.sendToServer();
}

void PlayerMessageHelper::writePlayerIntoMessage(NetworkMessageWriter& msg, Player* player)
{
    Q_ASSERT(nullptr != player);
    if(nullptr == player)
        return;

    msg.string16(player->name());
    msg.string8(player->uuid());
    auto color= player->getColor();
    msg.rgb(color.rgb());
    msg.uint8(player->isGM() ? 1 : 0);
    msg.string16(QCoreApplication::instance()->applicationVersion());

    auto avatar= player->getAvatar();

    msg.uint8(static_cast<quint8>(!avatar.isNull()));
    if(!avatar.isNull())
    {
        QByteArray baImage;
        QBuffer bufImage(&baImage);
        if(avatar.save(&bufImage, "PNG", 70))
        {
            msg.byteArray32(baImage);
        }
    }

    const auto& characters= player->children();
    // Characters
    msg.int32(static_cast<int>(characters.size()));

    std::for_each(characters.begin(), characters.end(), [&msg](const std::unique_ptr<Character>& character) {
        writeCharacterIntoMessage(msg, character.get());
    });

    /*QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out << player->features();

    msg.byteArray32(array);*/
}

void PlayerMessageHelper::writeCharacterIntoMessage(NetworkMessageWriter& msg, Character* character)
{
    Q_ASSERT(nullptr != character);
    if(nullptr == character)
        return;

    auto parent= character->parentPerson();
    msg.string8(nullptr != parent ? parent->uuid() : QStringLiteral("nullptr"));
    msg.string8(character->uuid());
    msg.string16(character->name());
    msg.string16(character->stateId());
    msg.uint8(static_cast<quint8>(character->isNpc()));
    msg.int32(character->number());
    auto color= character->getColor();
    msg.rgb(color.rgb());
    msg.int32(character->getHealthPointsCurrent());
    msg.int32(character->getHealthPointsMin());
    msg.int32(character->getHealthPointsMax());
    msg.int32(character->getInitiativeScore());
    msg.string32(character->getInitCommand());
    msg.real(character->getDistancePerTurn());
    msg.uint8(static_cast<quint8>(character->hasInitScore()));

    auto avatar= character->getAvatar();

    msg.uint8(static_cast<quint8>(!avatar.isNull()));
    if(!avatar.isNull())
    {
        QByteArray baImage;
        QBuffer bufImage(&baImage);
        if(avatar.save(&bufImage, "PNG", 70))
        {
            msg.byteArray32(baImage);
            qDebug() << "writeCharacterIntoMessage image data: " << baImage.size()
                     << "size of message:" << msg.getDataSize() << msg.action() << " categoriy" << msg.category();
        }
    }
}

bool PlayerMessageHelper::readPlayer(NetworkMessageReader& msg, Player* player)
{
    if(!msg.isValid() || nullptr == player)
    {
        qWarning() << "Network message OUT OF MEMORY player";
        return false;
    }

    auto name= msg.string16();
    auto uuid= msg.string8();
    auto color= QColor(msg.rgb());
    auto gameMaster= (msg.uint8() != 0);
    auto softVersion= msg.string16();

    // auto player= new Player(uuid, name, color, gameMaster);
    qDebug() << player->name() << "new player name" << name;
    player->setUuid(uuid);
    player->setName(name);
    player->setColor(color);
    player->setGM(gameMaster);
    player->setUserVersion(softVersion);

    bool hasAvatar= static_cast<bool>(msg.uint8());
    if(hasAvatar)
    {
        auto avatar= QImage::fromData(msg.byteArray32());
        player->setAvatar(avatar);
    }

    int childCount= msg.int32();
    qDebug() << "character count: " << childCount;
    for(int i= 0; (i < childCount && msg.isValid()); ++i)
    {
        try
        {
            Character* child= readCharacter(msg);
            player->addCharacter(child);
        }
        catch(std::bad_alloc&)
        {
            qWarning() << "Bad alloc";
            return false;
        }
    }
    if(!msg.isValid())
    {
        qWarning() << "Network message OUT OF MEMORY player after character";
        return false;
    }
    /*QByteArray array= msg.byteArray32();
    QDataStream in(&array, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_7);
    QMap<QString, quint8> features;
    in >> features;
    for(auto key : features.keys())
    {
        auto value= features.value(key);
        player->setFeature(key, value);
    }*/
    return true;
}

Character* PlayerMessageHelper::readCharacter(NetworkMessageReader& msg)
{
    auto character= new Character();
    if(!msg.isValid())
        return {};
    QString parentId= msg.string8();
    character->setUuid(msg.string8());
    character->setName(msg.string16());
    character->setStateId(msg.string16());
    character->setNpc(static_cast<bool>(msg.uint8()));
    character->setNumber(msg.int32());
    character->setColor(QColor(msg.rgb()));
    character->setHealthPointsCurrent(msg.int32());
    character->setHealthPointsMin(msg.int32());
    character->setHealthPointsMax(msg.int32());
    character->setInitiativeScore(msg.int32());
    character->setInitCommand(msg.string32());
    character->setDistancePerTurn(msg.real());
    character->setHasInitiative(static_cast<bool>(msg.uint8()));

    bool hasAvatar= static_cast<bool>(msg.uint8());

    if(hasAvatar)
    {
        auto byte= msg.byteArray32();
        qDebug() << "readCharacter image data: " << byte.size() << "size of message:" << msg.getSize() << msg.action()
                 << " categoriy" << msg.category();

        auto avatar= QImage::fromData(byte, "PNG");
        qDebug() << "avatar is null" << avatar.isNull() << character->name();
        character->setAvatar(avatar);
    }

    return character;
}
