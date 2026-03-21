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
#ifndef PLAYERMESSAGEHELPER_H
#define PLAYERMESSAGEHELPER_H

#include <QByteArray>
#include <QJsonObject>
#include <network_global.h>

class Player;
class NetworkMessageWriter;
class NetworkMessageReader;
class Character;
class PlayerModel;
class ClientManager;
class CharacterVision;
class Channel;
class NETWORK_EXPORT PlayerMessageHelper
{
public:
    static void sendOffConnectionInfo(const QString& playerId, const QString& playerName, const QByteArray& password);
    static void writePlayerIntoMessage(NetworkMessageWriter& msg, Player* player);
    static void writeCharacterIntoMessage(NetworkMessageWriter& msg, Character* character);
    static void writeVisionIntoMessage(NetworkMessageWriter& msg, CharacterVision* vision);
    static void sendOffPlayerInformations(Player* player);

    static bool readPlayer(NetworkMessageReader& msg, Player* player);
    static Character* readCharacter(NetworkMessageReader& msg, QString& parentId);

    static QJsonObject readChannelInMsg(NetworkMessageReader& msg);
    static void writeChannelInMsg(NetworkMessageWriter& msg, const QString& id, const QString& name,
                                  const QString& desc);

    static void fetchUserLeftChannelMsg(NetworkMessageWriter& msg, const QString& channelId, const QString& userId,
                                        const QByteArray& password);
};

#endif // PLAYERMESSAGEHELPER_H
