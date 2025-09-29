/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                                 *
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
#include "worker/fileserializer.h"

#include "dice3dcontroller.h"
#include "diceparser/dicealias.h"
#include "model/dicealiasmodel.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent>

namespace dice3D
{

namespace
{
constexpr auto command{"command"};
constexpr auto comment{"comment"};
constexpr auto pattern{"pattern"};
constexpr auto enabled{"enabled"};
constexpr auto replacement{"replacement"};
} // namespace

namespace jsonkey3DDice
{
constexpr auto colorD4{"colorD4"};
constexpr auto colorD6{"colorD6"};
constexpr auto colorD8{"colorD8"};
constexpr auto colorD10{"colorD10"};
constexpr auto colorD12{"colorD12"};
constexpr auto colorD20{"colorD20"};
constexpr auto colorD100{"colorD100"};
constexpr auto displayed{"displayed"};
constexpr auto muted{"muted"};
constexpr auto commandPart{"commandPart"};
constexpr auto shared{"sharedOnline"};
constexpr auto hideTime{"hideTime"};
constexpr auto model{"model"};
constexpr auto factor{"factor"};
} // namespace jsonkey3DDice
namespace FileSerializer
{
QJsonObject diceAliasToJSonObject(DiceAlias* alias)
{
    QJsonObject aliasJson;
    aliasJson[command]= alias->command();
    aliasJson[comment]= alias->comment();
    aliasJson[pattern]= alias->pattern();
    aliasJson[enabled]= !alias->isDisable();
    aliasJson[replacement]= alias->isReplace();
    return aliasJson;
}
QJsonArray dicesToArray(const std::vector<std::unique_ptr<DiceAlias>>& vec)
{
    QJsonArray array;
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(array),
                   [](const std::unique_ptr<DiceAlias>& alias)
                   { return FileSerializer::diceAliasToJSonObject(alias.get()); });
    return array;
}

QByteArray jsonArrayToByteArray(const QJsonArray& obj)
{
    QJsonDocument doc;
    doc.setArray(obj);
    return doc.toJson(QJsonDocument::Indented);
}

QByteArray buildDice3dData(Dice3DController* ctrl)
{
    namespace sj= dice3D::jsonkey3DDice;

    QJsonObject obj;

    obj[sj::colorD4]= ctrl->fourColor().name();
    obj[sj::colorD6]= ctrl->sixColor().name();
    obj[sj::colorD8]= ctrl->eightColor().name();
    obj[sj::colorD10]= ctrl->tenColor().name();
    obj[sj::colorD12]= ctrl->twelveColor().name();
    obj[sj::colorD20]= ctrl->twentyColor().name();
    obj[sj::colorD100]= ctrl->oneHundredColor().name();

    obj[sj::displayed]= ctrl->displayed();
    obj[sj::factor]= ctrl->factor();
    obj[sj::muted]= ctrl->muted();
    obj[sj::commandPart]= ctrl->commandPart();
    obj[sj::shared]= ctrl->sharedOnline();
    obj[sj::hideTime]= ctrl->hideTime();
    auto const& model= ctrl->model()->localModel();
    QList<DiceController*> dices;
    std::transform(std::begin(model), std::end(model), std::back_inserter(dices),
                   [](const std::unique_ptr<DiceController>& ctrl) { return ctrl.get(); });
    auto bytes= DiceModel::diceControllerToData(dices, QSize{2, 2});
    auto doc= QJsonDocument::fromJson(bytes);
    obj[sj::model]= doc.array();

    QJsonDocument output;
    output.setObject(obj);
    return output.toJson();
}
bool fetchDice3d(Dice3DController* ctrl, const QByteArray& data)
{
    if(data.isEmpty())
        return false;

    auto docu= QJsonDocument::fromJson(data);

    auto obj= docu.object();
    namespace sj= dice3D::jsonkey3DDice;
    ctrl->setFourColor(obj[sj::colorD4].toString());
    ctrl->setSixColor(obj[sj::colorD6].toString());
    ctrl->setEightColor(obj[sj::colorD8].toString());
    ctrl->setTenColor(obj[sj::colorD10].toString());
    ctrl->setTwelveColor(obj[sj::colorD12].toString());
    ctrl->setTwentyColor(obj[sj::colorD20].toString());
    ctrl->setOneHundredColor(obj[sj::colorD100].toString());
    ctrl->setDisplayed(obj[sj::displayed].toBool(false));
    ctrl->setMuted(obj[sj::muted].toBool(false));
    ctrl->setCommandPart(obj[sj::commandPart].toString());
    ctrl->setSharedOnline(obj[sj::shared].toBool(false));
    ctrl->setFactor(obj[sj::factor].toDouble(32.0));
    ctrl->setHideTime(obj[sj::hideTime].toInt(30));

    auto array= obj[sj::model].toArray();
    std::vector<std::unique_ptr<DiceController>> temp;
    DiceModel::fetchModel(jsonArrayToByteArray(array), temp, QSize{2, 2});
    auto model= ctrl->model();

    for(auto& dice : temp)
    {
        model->addDice(dice.release());
    }
    return true;
}

void fetchDiceModel(const QJsonArray& dice, DiceAliasModel* model)
{
    for(const auto& obj : dice)
    {
        auto diceCmd= obj.toObject();

        DiceAlias alias(diceCmd[pattern].toString(), diceCmd[command].toString(), diceCmd[comment].toString(),
                        diceCmd[replacement].toBool(), !diceCmd[enabled].toBool());

        model->appendAlias(std::move(alias));
    }
}

QJsonArray byteArrayToJsonArray(const QByteArray& data)
{
    QJsonDocument doc= QJsonDocument::fromJson(data);
    return doc.array();
}
} // namespace FileSerializer
} // namespace dice3D
