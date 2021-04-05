/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
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
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMimeData>
#include <chrono>

#include "playerwidget.h"

#include "network/networkmessage.h"
#include "ui_audiowidget.h"
#define FACTOR_WAIT 4

QString getExistingFile(const QString& rootDir, const QString& pathOnGM)
{
    QFileInfo info(pathOnGM);

    auto list= pathOnGM.split("/");
    QStringList result;
    for(auto& item : list)
    {
        result.prepend(item);
    }
    QString consumedPath= "";

    for(auto& item : result)
    {
        if(consumedPath.isEmpty())
            consumedPath= item;
        else
            consumedPath= item + "/" + consumedPath;
        auto temp= QStringLiteral("%1/%2").arg(rootDir, consumedPath);
        if(QFile::exists(temp))
        {
            return temp;
        }
    }
    return rootDir + "/" + info.fileName(); // error message
}

PlayerWidget::PlayerWidget(int id, QWidget* parent)
    : QWidget(parent), m_id(id), m_ui(new Ui::AudioWidgetUI), m_isGM(false)
{
    auto seed= std::chrono::high_resolution_clock::now().time_since_epoch().count();
    m_rng= std::mt19937(quintptr(this) + static_cast<unsigned long long>(seed));
    setAcceptDrops(true);

    m_preferences= PreferencesManager::getInstance();
    // m_preferences->registerLambda();
    m_ui->setupUi(this);
    setPlayingMode(NEXT);

    setupUi();
    m_model= new MusicModel(this);
    m_ui->m_songList->setModel(m_model);
    updateUi(true);
}

void PlayerWidget::startMedia(QMediaContent* p, QString title, bool play)
{
    m_content= p;
    m_player.setMedia(*m_content);
    m_ui->m_timeSlider->setMinimum(0);
    if(title.isEmpty())
    {
        m_ui->m_label->setText(p->request().url().fileName());
    }
    else
    {
        m_ui->m_label->setText(title);
    }
    if(play)
    {
        m_player.play();
    }
}
void PlayerWidget::setDuration(qint64 duration)
{
    m_ui->m_timeSlider->setMaximum(static_cast<int>(duration));
}

void PlayerWidget::positionChanged(qint64 time)
{
    QTime displayTime(0, static_cast<int>((time / 60000) % 60), static_cast<int>((time / 1000) % 60));

    if(m_isGM && ((time > m_time + (FACTOR_WAIT * m_player.notifyInterval())) || (time < m_time)))
    {
        emit playerPositionChanged(m_id, m_time);
    }
    m_time= time;
    m_ui->m_timeSlider->setValue(static_cast<int>(time));
    m_ui->m_timerDisplay->display(displayTime.toString("mm:ss"));
}
void PlayerWidget::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch(status)
    {
    case QMediaPlayer::EndOfMedia:
        findNext();
        break;
    default:
        break;
    }
}
void PlayerWidget::findNext()
{
    switch(m_playingMode)
    {
    case NEXT:
    {
        QModelIndex index= m_model->getCurrentSong();
        int next= index.row() + 1;
        QModelIndex newIndex= index.sibling(next, 0);
        if(newIndex.isValid())
        {
            startMediaByModelIndex(newIndex);
        }
    }
    break;
    case UNIQUE:
        // nothing
        break;
    case SHUFFLE:
    {
        QModelIndex current= m_model->getCurrentSong();
        auto size= m_model->rowCount();
        if(size > 1)
        {
            std::uniform_int_distribution<qint64> dist(0, size - 1);
            int value= current.row();
            while(value == current.row())
            {
                value= static_cast<int>(dist(m_rng));
            }
            QModelIndex newIndex= current.sibling(value, 0);
            if(newIndex.isValid())
            {
                startMediaByModelIndex(newIndex);
            }
        }
    }
    break;
    case LOOP:
        m_player.play();
        emit playerIsPlaying(m_id, 0);
        break;
    }
}

void PlayerWidget::setupUi()
{
    m_ui->m_timeSlider->setMinimum(0);
    m_ui->m_volumeSlider->setRange(0, 100);
    // **************** CREATE ACTIONS ********************************
    m_playAct= new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    m_pauseAct= new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    m_stopAct= new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);

    m_uniqueAct= new QAction(QIcon::fromTheme("playunique"), tr("Stop at the end"), this);
    m_uniqueAct->setShortcut(QKeySequence("Ctrl+U"));
    m_uniqueAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_uniqueAct->setCheckable(true);

    m_repeatAct= new QAction(QIcon::fromTheme("playloop"), tr("Play in loop"), this);
    m_repeatAct->setCheckable(true);
    m_repeatAct->setShortcut(QKeySequence("Ctrl+R"));
    m_repeatAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_shuffleAct= new QAction(QIcon::fromTheme("shuffle_mode"), tr("Shuffle Mode"), this);
    m_shuffleAct->setCheckable(true);
    m_shuffleAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_changeDirectoryAct= new QAction(style()->standardIcon(QStyle::SP_DirIcon), tr("Open Directory"), this);
    m_volumeMutedAct= new QAction(tr("Mute Volume"), this);
    m_volumeMutedAct->setCheckable(true);
    m_volumeMutedAct->setShortcut(QKeySequence("Ctrl+M"));
    m_volumeMutedAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_loadTableTopAudioPlayListAct= new QAction(tr("load TableTopAudio.com playlist"), this);

    m_openPlayList= new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), tr("Open Playlist"), this);
    m_openPlayList->setShortcut(QKeySequence("Ctrl+J"));
    m_openPlayList->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_savePlayList= new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save Playlist"), this);
    m_savePlayList->setShortcut(QKeySequence("Ctrl+E"));
    m_savePlayList->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_clearList= new QAction(style()->standardIcon(QStyle::SP_DialogResetButton), tr("Clear"), this);
    m_clearList->setShortcut(QKeySequence("Ctrl+Del"));
    m_clearList->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_addAction= new QAction(QIcon::fromTheme("add"), tr("Add Songs"), this);
    m_addAction->setShortcut(QKeySequence("Ctrl+A"));
    m_addAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_addStreamAction= new QAction(tr("Open Stream"), this);

    m_deleteAction= new QAction(QIcon::fromTheme("remove"), tr("Remove Song"), this);
    m_deleteAction->setShortcut(QKeySequence("Del"));
    m_deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_ui->m_volumeSlider->setValue(m_preferences->value(QString("volume_player_%1").arg(m_id), 50).toInt());
    m_audioFileFilter= m_preferences->value("AudioFileFilter", "*.wav *.mp2 *.mp3 *.ogg *.flac").toString();

    // **************** Add ACTIONS ********************************

    addAction(m_uniqueAct);
    addAction(m_repeatAct);
    addAction(m_shuffleAct);
    addAction(m_volumeMutedAct);
    addAction(m_openPlayList);
    addAction(m_savePlayList);
    addAction(m_clearList);
    addAction(m_deleteAction);
    addAction(m_addAction);
    addAction(m_addStreamAction);

    // **************** TOOLTIP ACTIONS ********************************
    m_addAction->setToolTip(tr("Add song to the list"));
    m_deleteAction->setToolTip(tr("Remove selected file"));

    // **************** Add Action In Buttons ********************************
    m_ui->m_deleteButton->setDefaultAction(m_deleteAction);
    m_ui->m_deleteButton->addAction(m_clearList);
    m_ui->m_savePlaylist->setDefaultAction(m_savePlayList);
    m_ui->m_addButton->setDefaultAction(m_addAction);
    m_ui->m_addButton->addAction(m_openPlayList);
    m_ui->m_addButton->addAction(m_loadTableTopAudioPlayListAct);
    m_ui->m_addButton->addAction(m_addStreamAction);

    m_ui->m_volumeMutedButton->setDefaultAction(m_volumeMutedAct);
    m_ui->m_playButton->setDefaultAction(m_playAct);
    m_ui->m_pauseButton->setDefaultAction(m_pauseAct);
    m_ui->m_stopButton->setDefaultAction(m_stopAct);
    m_ui->m_uniqueMode->setDefaultAction(m_uniqueAct);
    m_ui->m_shuffleBtn->setDefaultAction(m_shuffleAct);
    m_ui->m_repeatMode->setDefaultAction(m_repeatAct);
    m_ui->m_changeDirectory->setDefaultAction(m_changeDirectoryAct);

    // **************** CONNECT ********************************
    connect(m_addAction, SIGNAL(triggered(bool)), this, SLOT(addFiles()));
    connect(m_openPlayList, SIGNAL(triggered(bool)), this, SLOT(openPlayList()));
    connect(m_addStreamAction, SIGNAL(triggered(bool)), this, SLOT(openStream()));
    connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(removeFile()));
    connect(m_clearList, SIGNAL(triggered(bool)), this, SLOT(removeAll()));
    connect(m_ui->m_songList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(startMediaByModelIndex(QModelIndex)));
    updateIcon();
    connect(&m_player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(&m_player, SIGNAL(durationChanged(qint64)), this, SLOT(setDuration(qint64)));
    connect(m_playAct, SIGNAL(triggered()), this, SLOT(playSelectedSong()));

    connect(m_stopAct, SIGNAL(triggered()), &m_player, SLOT(stop()));
    connect(m_pauseAct, SIGNAL(triggered()), &m_player, SLOT(pause()));
    connect(m_ui->m_timeSlider, SIGNAL(sliderMoved(int)), this, SLOT(setTime(int)));
    connect(m_ui->m_volumeSlider, SIGNAL(valueChanged(int)), &m_player, SLOT(setVolume(int)));
    connect(m_ui->m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(saveVolumeValue(int)));
    connect(&m_player, SIGNAL(currentMediaChanged(QMediaContent)), this, SLOT(sourceChanged(QMediaContent)));
    connect(&m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStatusChanged(QMediaPlayer::State)));
    connect(m_volumeMutedAct, SIGNAL(triggered(bool)), &m_player, SLOT(setMuted(bool)));
    connect(m_volumeMutedAct, SIGNAL(triggered(bool)), this, SLOT(updateIcon()));
    connect(m_changeDirectoryAct, SIGNAL(triggered()), this, SLOT(changeDirectory()));
    connect(&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this,
            SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(&m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(errorOccurs(QMediaPlayer::Error)));
    connect(m_ui->m_label, SIGNAL(textChanged(QString)), this, SLOT(labelTextChanged()));

    connect(m_repeatAct, &QAction::triggered, this, &PlayerWidget::triggeredPlayingModeAction);
    connect(m_uniqueAct, &QAction::triggered, this, &PlayerWidget::triggeredPlayingModeAction);
    connect(m_shuffleAct, &QAction::triggered, this, &PlayerWidget::triggeredPlayingModeAction);

    connect(m_loadTableTopAudioPlayListAct, SIGNAL(triggered()), this, SLOT(loadPlayList()));
    connect(m_savePlayList, SIGNAL(triggered()), this, SLOT(savePlaylist()));
}
void PlayerWidget::startMediaByModelIndex(QModelIndex p) // double click
{
    startMedia(m_model->getMediaByModelIndex(p), m_model->data(p).toString());
    m_model->setCurrentSong(p);
    //  m_mediaObject->play();
}

const MusicModel* PlayerWidget::model() const
{
    return m_model;
}

void PlayerWidget::removeFile()
{
    QModelIndexList list= m_ui->m_songList->selectionModel()->selectedIndexes();
    m_model->removeSong(list);
}
void PlayerWidget::currentChanged(const QModelIndex& current, const QModelIndex&)
{
    if((current.isValid()) && (m_player.mediaStatus() == QMediaPlayer::NoMedia))
    {
        startMedia(m_model->getMediaByModelIndex(current), current.data().toString(), false);
    }
}
void PlayerWidget::playSelectedSong()
{
    QModelIndex current= m_ui->m_songList->currentIndex();
    if((current.isValid())
       && ((m_player.mediaStatus() == QMediaPlayer::NoMedia) || (m_player.mediaStatus() == QMediaPlayer::EndOfMedia)
           || (m_player.state() == QMediaPlayer::StoppedState)))
    {
        startMedia(m_model->getMediaByModelIndex(current), current.data().toString());
    }
    else
    {
        m_player.play();
    }
}
void PlayerWidget::addSongIntoModel(QString str)
{
    QStringList songs;
    songs << str;
    m_model->addSong(songs);
}

void PlayerWidget::addFiles()
{
    QStringList fileList= QFileDialog::getOpenFileNames(
        this, tr("Add song"),
        m_preferences->value(QStringLiteral("MusicDirectoryPlayer_%1").arg(m_id), QDir::homePath()).toString(),
        tr("Audio files (%1)").arg(m_audioFileFilter));
    if(fileList.isEmpty())
        return;
    m_model->addSong(fileList);
}
bool PlayerWidget::askToDeleteAll()
{
    if(m_model->rowCount() != 0)
    {
        if(QMessageBox::Ok
            == QMessageBox::warning(this, tr("Attention!"),
                   tr("You are about to load an new playlist. All previously load file will be dropped."),
                   QMessageBox::Ok, QMessageBox::Cancel))
        {
            m_model->removeAll();
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}
void PlayerWidget::openPlayList()
{
    if(askToDeleteAll())
    {
        QString filename= QFileDialog::getOpenFileName(
            this, tr("Open Play List"),
            m_preferences->value(QStringLiteral("MusicDirectoryPlayer_%1").arg(m_id), QDir::homePath()).toString(),
            tr("PlayList (*.m3u)"));
        if(filename.isEmpty())
            return;
        readM3uPlayList(filename);
    }
}
void PlayerWidget::openStream()
{
    bool a;
    QString value= QInputDialog::getText(this, tr("Open audio Stream"), tr("URL"), QLineEdit::Normal, QString(), &a);
    if(a)
    {
        QUrl url(value);
        if(url.isValid())
        {
            QStringList values;
            values << value;
            m_model->addSong(values);
        }
    }
}

void PlayerWidget::readM3uPlayList(QString filepath)
{
    QFile file(filepath);
    /// @todo make this job in thread.
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream read(&file);
        QString line;
        QStringList result;
        while(!read.atEnd())
        {
            line= read.readLine();
            if(!line.startsWith("#EXTINF", Qt::CaseSensitive))
            {
                result.append(line);
            }
        }
        m_model->addSong(result);
        emit newPlaylistLoaded(filepath);
    }
}
void PlayerWidget::contextMenuEvent(QContextMenuEvent* ev)
{
    ev->ignore();
}
void PlayerWidget::addActionsIntoMenu(QMenu* menu)
{
    menu->addAction(m_playAct);
    menu->addAction(m_pauseAct);
    menu->addAction(m_stopAct);
    menu->addSeparator();
    menu->addAction(m_addAction);
    menu->addAction(m_addStreamAction);
    menu->addAction(m_openPlayList);
    menu->addAction(m_loadTableTopAudioPlayListAct);
    menu->addAction(m_savePlayList);
    menu->addAction(m_clearList);
    menu->addAction(m_deleteAction);
    menu->addSeparator();
}

void PlayerWidget::updateUi(bool isGM)
{
    m_isGM= isGM;
    m_ui->m_playButton->setVisible(isGM);
    m_ui->m_stopButton->setVisible(isGM);
    m_ui->m_pauseButton->setVisible(isGM);
    m_ui->m_uniqueMode->setVisible(isGM);
    m_ui->m_repeatMode->setVisible(isGM);
    m_ui->m_shuffleBtn->setVisible(isGM);
    m_ui->m_timeSlider->setVisible(isGM);
    m_ui->m_addButton->setVisible(isGM);
    m_ui->m_deleteButton->setVisible(isGM);
    m_ui->m_songList->setVisible(isGM);
    m_ui->m_savePlaylist->setVisible(isGM);
    m_ui->m_changeDirectory->setVisible(!isGM);
    m_ui->m_timerDisplay->setVisible(isGM);
    m_ui->m_volumeSlider->setValue(m_preferences->value(QString("volume_player_%1").arg(m_id), 50).toInt());
}

void PlayerWidget::updateIcon()
{
    if(m_volumeMutedAct->isChecked())
    {
        m_volumeMutedAct->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else
    {
        m_volumeMutedAct->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}
void PlayerWidget::setTime(int time)
{
    m_player.setPosition(time);
}
void PlayerWidget::sourceChanged(const QMediaContent& media)
{
    emit newSongPlayed(m_id, media.request().url().toString());
}
void PlayerWidget::playerStatusChanged(QMediaPlayer::State newState)
{
    switch(newState)
    {
    case QMediaPlayer::StoppedState:
        emit playerStopped(m_id);
        break;
    case QMediaPlayer::PlayingState:
        emit playerIsPlaying(m_id, m_player.position());
        break;
    case QMediaPlayer::PausedState:
        emit playerIsPaused(m_id);
        break;
    }
}
void PlayerWidget::saveVolumeValue(int volume)
{
    m_preferences->registerValue(QString("volume_player_%1").arg(m_id), volume, true);
}
void PlayerWidget::removeAll()
{
    m_model->removeAll();
}
void PlayerWidget::playSong(qint64 pos)
{
    setPositionAt(pos);
    m_player.play();
}

void PlayerWidget::stop()
{
    m_player.stop();
}

void PlayerWidget::pause()
{
    m_player.pause();
}

void PlayerWidget::setPositionAt(qint64 pos)
{
    m_player.setPosition(pos);
}

void PlayerWidget::setSourceSong(QString file)
{
    //qDebug() << "file" << file;

    QString dir= m_preferences->value(QStringLiteral("MusicDirectoryPlayer_%1").arg(m_id), QDir::homePath()).toString();
    QUrl url(file, QUrl::StrictMode);
    if((!url.isValid()) || (url.isRelative()) || url.isLocalFile())
    {
        url= QUrl::fromLocalFile(getExistingFile(dir, file));
    }
    QMediaContent currentContent(url);

    m_ui->m_label->setText(file);
    m_player.setMedia(currentContent);
}
void PlayerWidget::changeDirectory()
{
    QString dir= QFileDialog::getExistingDirectory(
        this, tr("Load Directory"),
        m_preferences->value(QString("MusicDirectoryPlayer_%1").arg(m_id), QDir::homePath()).toString());
    if(!dir.isEmpty())
    {
        m_preferences->registerValue(QStringLiteral("MusicDirectoryPlayer_%1").arg(m_id), dir, true);
    }
}

void PlayerWidget::triggeredPlayingModeAction()
{
    QAction* act= qobject_cast<QAction*>(sender());
    bool status= act->isChecked();
    if(status)
    {
        if(m_uniqueAct == act)
        {
            m_repeatAct->setChecked(false);
            m_shuffleAct->setChecked(false);
        }
        else if(m_repeatAct == act)
        {
            m_uniqueAct->setChecked(false);
            m_shuffleAct->setChecked(false);
        }
        else if(m_shuffleAct == act)
        {
            m_uniqueAct->setChecked(false);
            m_repeatAct->setChecked(false);
        }
    }
    if(m_repeatAct->isChecked())
    {
        setPlayingMode(LOOP);
    }
    else if(m_uniqueAct->isChecked())
    {
        setPlayingMode(UNIQUE);
    }
    else if(m_shuffleAct->isChecked())
    {
        setPlayingMode(SHUFFLE);
    }
    else
    {
        setPlayingMode(NEXT);
    }
}

void PlayerWidget::setPlayingMode(PlayerWidget::PlayingMode mode)
{
    m_playingMode= mode;
}
void PlayerWidget::loadPlayList()
{
    QString url = QStringLiteral("http://tabletopaudio.com/download.php?downld_file=%1");
    static QStringList list({
        QStringLiteral("1_The_Inner_Core.mp3"),
        QStringLiteral("2_Bubbling_Pools.mp3"),
        QStringLiteral("3_The_March_of_the_Faithful.mp3"),
        QStringLiteral("4_Solemn_Vow-a.mp3"),
        QStringLiteral("5_Desert_Bazaar.mp3"),
        QStringLiteral("6_Abyssal_Gaze.mp3"),
        QStringLiteral("7_The_Desert_Awaits.mp3"),
        QStringLiteral("8_New_Dust_to_Dust.mp3"),
        QStringLiteral("9_Before_The_Storm.mp3"),
        QStringLiteral("10_In_The_Shadows.mp3"),
        QStringLiteral("11_Shelter_from_the_Storm.mp3"),
        QStringLiteral("12_Disembodied_Spirits.mp3"),
        QStringLiteral("13_Cave_of_Time.mp3"),
        QStringLiteral("14_Protean_Fields.mp3"),
        QStringLiteral("15_Alien_Machine_Shop.mp3"),
        QStringLiteral("16_Busy_Space_Port.mp3"),
        QStringLiteral("17_Alien_Night_Club.mp3"),
        QStringLiteral("18_House_on_the_Hill.mp3"),
        QStringLiteral("19_Age_of_Sail.mp3"),
        QStringLiteral("20_Dark_Continent_aa.mp3"),
        QStringLiteral("21_Derelict_Freighter.mp3"),
        QStringLiteral("22_True_West_a.mp3"),
        QStringLiteral("23_The_Slaughtered_Ox.mp3"),
        QStringLiteral("24_Forbidden_Galaxy.mp3"),
        QStringLiteral("25_Deep_Space_EVA.mp3"),
        QStringLiteral("26_Uncommon_Valor_a.mp3"),
        QStringLiteral("27_Xingu_Nights.mp3"),
        QStringLiteral("28_Nephilim_Labs_FE.mp3"),
        QStringLiteral("29_Kaltoran_Craft_FE.mp3"),
        QStringLiteral("30_Los_Vangeles_3030.mp3"),
        QStringLiteral("31_Frozen_Wastes.mp3"),
        QStringLiteral("32_City_and_the_City.mp3"),
        QStringLiteral("33_Far_Above_the_World.mp3"),
        QStringLiteral("34_Clash_of_Kings.mp3"),
        QStringLiteral("35_Swamplandia.mp3"),
        QStringLiteral("36_Down_by_the_Sea.mp3"),
        QStringLiteral("37_Catacombs.mp3"),
        QStringLiteral("38_Into_the_Deep.mp3"),
        QStringLiteral("39_Temple_of_the_Eye.mp3"),
        QStringLiteral("40_The_Long_Rain.mp3"),
        QStringLiteral("41_Starship_Bridge.mp3"),
        QStringLiteral("42_Rise_of_the_Ancients.mp3"),
        QStringLiteral("43_Dome_City_Center.mp3"),
        QStringLiteral("44_Victorian_London.mp3"),
        QStringLiteral("45_Samurai_HQ.mp3"),
        QStringLiteral("46_Cathedral.mp3"),
        QStringLiteral("47_There_be_Dragons.mp3"),
        QStringLiteral("48_Overland_with_Oxen.mp3"),
        QStringLiteral("49_Goblin's_Cave.mp3"),
        QStringLiteral("50_Super_Hero.mp3"),
        QStringLiteral("51_Woodland_Campsite.mp3"),
        QStringLiteral("52_Warehouse_13.mp3"),
        QStringLiteral("53_Strangers_on_a_Train.mp3"),
        QStringLiteral("54_Mountain_Tavern.mp3"),
        QStringLiteral("55_Ice_Cavern.mp3"),
        QStringLiteral("56_Medieval_Town.mp3"),
        QStringLiteral("57_Colosseum.mp3"),
        QStringLiteral("58_Terror.mp3"),
        QStringLiteral("59_Dinotopia.mp3"),
        QStringLiteral("60_Dark_and_Stormy.mp3"),
        QStringLiteral("61_Orbital_Platform.mp3"),
        QStringLiteral("62_Middle_Earth_Dawn.mp3"),
        QStringLiteral("63_Industrial_Shipyard.mp3"),
        QStringLiteral("64_Mountain_Pass.mp3"),
        QStringLiteral("65_Dungeon_I.mp3"),
        QStringLiteral("66_Royal_Salon.mp3"),
        QStringLiteral("67_Asylum.mp3"),
        QStringLiteral("68_1940s_Office.mp3"),
        QStringLiteral("69_Forest_Night.mp3"),
        QStringLiteral("70_Age_of_Steam.mp3")
    });

    // This is slower, but makes the above code *much* more readable.
    static int initOnce = true;
    if (initOnce) {
        for(QString& str : list) {
            str.prepend(url);
        }
        initOnce = false;
    }

    if(askToDeleteAll())
    {
        m_model->removeAll();
        m_model->addSong(list);
    }
}
void PlayerWidget::savePlaylist()
{
    QString filename= QFileDialog::getSaveFileName(
        this, tr("Save Play List"), m_preferences->value("MusicDirectoryGM", QDir::homePath()).toString(),
        tr("PlayList (*.m3u)"));
    if(filename.isEmpty())
        return;

    if(!filename.endsWith(".m3u"))
    {
        filename.append(".m3u");
    }
    QFile file(filename);

    // if(file.isWritable())
    {
        file.open(QIODevice::WriteOnly);
        QTextStream in(&file);
        m_model->saveIn(in);
    }
}
void PlayerWidget::errorOccurs(QMediaPlayer::Error e)
{
    if(QMediaPlayer::NoError == e)
        return;

    QString Error("Error %1 : %2");
    m_ui->m_label->setText(Error.arg(m_player.errorString(), m_player.currentMedia().request().url().toString()));
}
void PlayerWidget::labelTextChanged()
{
    if(m_ui->m_label->text().startsWith("Error") && m_player.error() != QMediaPlayer::NoError)
    {
        m_ui->m_label->setStyleSheet("color: red");
        m_ui->m_label->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        m_ui->m_label->setStyleSheet("color: black");
        if(!m_isGM)
        { // Player
            m_ui->m_label->setEchoMode(QLineEdit::Password);
        }
    }
}

void PlayerWidget::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData= event->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> list= mimeData->urls();
        for(auto& url : list)
        {
            if(url.toLocalFile().endsWith(".m3u")) // play list
            {
                readM3uPlayList(url.toLocalFile());
            }
            else
            {
                addSongIntoModel(url.toLocalFile());
            }
        }
        event->acceptProposedAction();
    }
}
