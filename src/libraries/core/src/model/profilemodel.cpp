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
#include "model/profilemodel.h"

#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QUuid>

// #include "data/character.h"
// #include "data/player.h"
#include "network/characterdatamodel.h"
#include "network/connectionprofile.h"
#include "worker/fileserializer.h"
#include "worker/utilshelper.h"

ProfileModel::ProfileModel() : m_characterModel{new CharacterDataModel} {}

ProfileModel::~ProfileModel()
{
    m_connectionProfileList.clear();
}

QVariant ProfileModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

int ProfileModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return static_cast<int>(m_connectionProfileList.size());
    return 0;
}
QVariant ProfileModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QVariant var;
    switch(role)
    {
    case Qt::DisplayRole:
    case NameRole:
        var= m_connectionProfileList.at(static_cast<std::size_t>(index.row()))->profileTitle();
        break;
    case IndexRole:
        var= index.row();
        break;
    default:
        break;
    }
    return var;
}

void ProfileModel::appendProfile()
{
    ConnectionProfile* profile= new ConnectionProfile();
    profile->setProfileTitle(QStringLiteral("Profile #%1").arg(m_connectionProfileList.size() + 1));
    appendProfile(profile);
}
void ProfileModel::appendProfile(ConnectionProfile* profile)
{
    if(nullptr == profile)
        return;

    if(profile->playerId().isEmpty())
        profile->setPlayerId(QUuid::createUuid().toString(QUuid::WithoutBraces));

    auto idx= static_cast<int>(m_connectionProfileList.size());

    connect(profile, &ConnectionProfile::titleChanged, profile,
            [this, profile]()
            {
                auto idx= indexOf(profile);
                emit dataChanged(index(idx, 0), index(idx, 0), {NameRole});
            });

    beginInsertRows(QModelIndex(), idx, idx);
    m_connectionProfileList.push_back(std::unique_ptr<ConnectionProfile>(std::move(profile)));
    endInsertRows();

    emit profileAdded(profile);
    checkProfile(profile);
}

void ProfileModel::checkProfile(ConnectionProfile* prof)
{
    if(prof == nullptr)
        return;

    connect(prof, &ConnectionProfile::gmChanged, this,
            [prof, this]()
            {
                if(!prof->isGM() && (prof->characterCount() == 0))
                {
                    connection::CharacterData data({QUuid::createUuid().toString(), QObject::tr("Unknown Character"),
                                                    Qt::red, "", QHash<QString, QVariant>()});
                    m_characterModel->addCharacter(data);
                }
            });

    auto updateCharacters
        = [prof]() { prof->setCharactersValid(helper::utils::hasValidCharacter(prof->characters(), prof->isGM())); };
    connect(prof, &ConnectionProfile::characterCountChanged, this, updateCharacters);
    connect(prof, &ConnectionProfile::characterChanged, this, updateCharacters);
    connect(prof, &ConnectionProfile::gmChanged, this, updateCharacters);

    auto updatePlayerInfo= [prof]()
    {
        prof->setPlayerInfoValid(prof->playerColor().isValid() && !prof->playerName().isEmpty()
                                 && helper::utils::isSquareImage(prof->playerAvatar()));
    };
    connect(prof, &ConnectionProfile::playerAvatarChanged, this, updatePlayerInfo);
    connect(prof, &ConnectionProfile::playerColorChanged, this, updatePlayerInfo);
    connect(prof, &ConnectionProfile::playerNameChanged, this, updatePlayerInfo);

    auto updateCampaign= [prof]()
    {
        prof->setCampaignInfoValid(
            prof->isGM() ? campaign::FileSerializer::isValidCampaignDirectory(prof->campaignPath()) : true);
    };
    connect(prof, &ConnectionProfile::gmChanged, this, updateCampaign);
    connect(prof, &ConnectionProfile::campaignPathChanged, this, updateCampaign);

    updateCharacters();
    updateCampaign();
    updatePlayerInfo();
}

ConnectionProfile* ProfileModel::getProfile(const QModelIndex& index)
{
    return getProfile(index.row());
}

int ProfileModel::cloneProfile(int index)
{
    auto profileSrc= getProfile(index);

    if(nullptr == profileSrc)
        return -1;

    ConnectionProfile* clonedProfile= new ConnectionProfile();
    clonedProfile->cloneProfile(profileSrc);
    auto name= clonedProfile->profileTitle();
    clonedProfile->setProfileTitle(name.append(tr(" (clone)")));
    appendProfile(clonedProfile);

    return indexOf(clonedProfile);
}

int ProfileModel::indexOf(ConnectionProfile* tmp)
{
    auto const& pos
        = std::find_if(m_connectionProfileList.begin(), m_connectionProfileList.end(),
                       [tmp](const std::unique_ptr<ConnectionProfile>& profile) { return tmp == profile.get(); });

    if(pos == m_connectionProfileList.end())
        return -1;

    auto idx= std::distance(m_connectionProfileList.begin(), pos);

    return static_cast<int>(idx);
}
ConnectionProfile* ProfileModel::getProfile(int index)
{
    ConnectionProfile* profile= nullptr;
    auto idx= static_cast<std::size_t>(index);
    if((!m_connectionProfileList.empty()) && (m_connectionProfileList.size() > idx))
    {
        profile= m_connectionProfileList.at(idx).get();
    }
    return profile;
}

QHash<int, QByteArray> ProfileModel::roleNames() const
{
    return {{NameRole, "name"}, {IndexRole, "uuid"}};
}
void ProfileModel::removeProfile(int index)
{
    auto idx= static_cast<std::size_t>(index);
    if(m_connectionProfileList.size() <= idx)
        return;

    auto pos= m_connectionProfileList.begin() + index;

    beginRemoveRows(QModelIndex(), index, index);
    m_connectionProfileList.erase(pos);
    endRemoveRows();

    emit profileRemoved(index);
}

Qt::ItemFlags ProfileModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

CharacterDataModel* ProfileModel::characterModel() const
{
    return m_characterModel.get();
}
