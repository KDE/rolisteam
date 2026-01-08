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
#ifndef CORE_FILESERIALIZER_H
#define CORE_FILESERIALIZER_H

#include <QFuture>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QStringList>

#include <memory>
#include <vector>

class DiceAlias;
class Dice3DController;
class DiceAliasModel;
namespace dice3D
{
namespace FileSerializer
{
QJsonArray dicesToArray(const std::vector<std::unique_ptr<DiceAlias>>& vec);
QJsonObject diceAliasToJSonObject(DiceAlias* alias);
QByteArray jsonArrayToByteArray(const QJsonArray& obj);
QByteArray buildDice3dData(Dice3DController* ctrl);
QJsonArray byteArrayToJsonArray(const QByteArray& data);
void fetchDiceModel(const QJsonArray& dice, DiceAliasModel* model);
bool fetchDice3d(Dice3DController* ctrl, const QByteArray& data);
}; // namespace FileSerializer
} // namespace dice3D

#endif // CORE_FILESERIALIZER_H
