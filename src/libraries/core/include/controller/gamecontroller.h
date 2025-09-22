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
#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QClipboard>
#include <QObject>
#include <QUndoStack>
#include <core_global.h>
#include <memory>

#include "controller/audiocontroller.h"
#include "controller/contentcontroller.h"
#include "controller/instantmessagingcontroller.h"
#include "controller/networkcontroller.h"
#include "controller/playercontroller.h"
#include "controller/preferencescontroller.h"
#include "data/campaign.h"
#include "data/campaignmanager.h"
#include "diceparser_qobject/diceroller.h"
#include "dicephysics/controllers/dice3dcontroller.h"
#include "updater/media/instantmessagingupdater.h"

struct TipOfDay
{
    QString title;
    QString content;
    QString url;
    int id;
};

namespace campaign
{
class CampaignManager;
class Campaign;
} // namespace campaign

class QAbstractItemModel;
class LogController;
class LogSenderScheduler;
class RemoteLogController;
class PreferencesManager;
class PlayerController;
class QSystemTrayIcon;
class AutoSaveController;
class CORE_EXPORT GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PreferencesController* preferencesController READ preferencesController CONSTANT)
    Q_PROPERTY(PlayerController* playerController READ playerController CONSTANT)
    Q_PROPERTY(ContentController* contentController READ contentController CONSTANT)
    Q_PROPERTY(AudioController* audioController READ audioController CONSTANT)
    Q_PROPERTY(NetworkController* networkController READ networkController CONSTANT)
    Q_PROPERTY(Dice3DController* dicePhysicController READ dicePhysicController CONSTANT)
    Q_PROPERTY(QString campaignRoot READ campaignRoot WRITE setCampaignRoot NOTIFY campaignRootChanged)
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QString localPlayerId READ localPlayerId NOTIFY localPlayerIdChanged)
    Q_PROPERTY(QString remoteVersion READ remoteVersion NOTIFY remoteVersionChanged)
    Q_PROPERTY(TipOfDay tipOfDay READ tipOfDay NOTIFY tipOfDayChanged)
    Q_PROPERTY(bool localIsGM READ localIsGM NOTIFY localIsGMChanged)
    Q_PROPERTY(bool updateAvailable READ updateAvailable WRITE setUpdateAvailable NOTIFY updateAvailableChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(DiceRoller* diceParser READ diceParser CONSTANT)
    Q_PROPERTY(campaign::CampaignManager* campaignManager READ campaignManager CONSTANT)
    Q_PROPERTY(campaign::Campaign* campaign READ campaign NOTIFY campaignChanged)
public:
    explicit GameController(const QString& appname, const QString& version, QClipboard* clipboard,
                            QObject* parent= nullptr);
    ~GameController();

    NetworkController* networkController() const;
    PlayerController* playerController() const;
    ContentController* contentController() const;
    PreferencesController* preferencesController() const;
    PreferencesManager* preferencesManager() const;
    InstantMessagingController* instantMessagingController() const;
    campaign::CampaignManager* campaignManager() const;
    campaign::Campaign* campaign() const;
    AudioController* audioController() const;

    QString version() const;
    QString campaignRoot() const;
    QString localPlayerId() const;
    QString remoteVersion() const;
    bool localIsGM() const;
    bool updateAvailable() const;
    bool tipAvailable() const;
    bool connected() const;
    QUndoStack* undoStack() const;
    DiceRoller* diceParser() const;

    LogController* logController() const;
    TipOfDay tipOfDay() const;

    void clear();

    Dice3DController* dicePhysicController() const;
    void setDicePhysicController(Dice3DController* newDicePhysicController);

signals:
    void campaignRootChanged();
    void versionChanged();
    void localPlayerIdChanged();
    void localIsGMChanged(bool);
    void updateAvailableChanged();
    void connectedChanged(bool b);
    void remoteVersionChanged();
    void tipOfDayChanged();
    void closingApp();
    void dataLoaded(const QStringList missingFiles, const QStringList unmanagedFiles);
    void campaignChanged();
    void dicePhysicControllerChanged();
    void themeChanged(const QString& theme);

public slots:
    void addErrorLog(const QString& message, const QString& cat);
    void addWarningLog(const QString& message, const QString& cat);
    void addFeatureLog(const QString& message, const QString& cat);
    void addInfoLog(const QString& message, const QString& cat);
    void addSearchLog(const QString& message, const QString& cat);

    void startCheckForUpdates();
    void startIpRetriever();
    void startTipOfDay();
    void postSettingInit();

    void setCampaignRoot(const QString& path);
    void setVersion(const QString& version);
    void setUpdateAvailable(bool available);
    void setDataFromProfile(int profileIndex);
    void startConnection();
    void stopConnection();
    void postConnection();
    void aboutToClose(bool save);
    void setLocalPlayerId(const QString& id);

    void newMedia(const std::map<QString, QVariant>& map);
    void openMedia(const std::map<QString, QVariant>& map);
    void openInternalResources(const QString& id, const QString& path, Core::ContentType type);

    void save();
    void saveAs(const QString& path);
    void openPageWeb(const QString& urlText);

private:
    void addCommand(QUndoCommand* cmd);

private:
    std::unique_ptr<DiceRoller> m_diceParser;
    std::unique_ptr<LogController> m_logController;
    std::unique_ptr<RemoteLogController> m_remoteLogCtrl;
    std::unique_ptr<NetworkController> m_networkCtrl;
    std::unique_ptr<PlayerController> m_playerController;
    std::unique_ptr<PreferencesController> m_preferencesDialogController;
    std::unique_ptr<campaign::CampaignManager> m_campaignManager;
    std::unique_ptr<ContentController> m_contentCtrl;
    std::unique_ptr<PreferencesManager> m_preferences;
    std::unique_ptr<InstantMessagingController> m_instantMessagingCtrl;
    std::unique_ptr<InstantMessaging::InstantMessagingUpdater> m_imUpdater;
    std::unique_ptr<AudioController> m_audioCtrl;
    std::unique_ptr<Dice3DController> m_dicePhysicController;

    QString m_version;
    QString m_remoteVersion;
    bool m_updateAvailable= false;
    TipOfDay m_tipOfTheDay;
    std::unique_ptr<QUndoStack> m_undoStack;
    std::unique_ptr<AutoSaveController> m_autoSaveCtrl;
};

#endif // GAMECONTROLLER_H
