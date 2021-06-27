/***************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                               *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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
#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H
#include <QContextMenuEvent>
#include <QMediaContent>
#include <QMediaPlayer>
#include <QMenu>
#include <QSlider>
#include <QTime>
#include <random>

#include "musicmodel.h"
#include "preferences/preferencesmanager.h"

namespace Ui
{
class AudioWidgetUI;
}

/**
 * @brief The PlayerWidget class manages all widgets requiered for playing music.
 */
class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    enum PlayingMode
    {
        LOOP,
        UNIQUE,
        NEXT,
        SHUFFLE
    };
    /**
     * @brief PlayerWidget
     * @param parent
     */
    PlayerWidget(int id, QWidget* parent= nullptr);
    /**
     * @brief startMedia
     */
    void startMedia(QMediaContent*, QString title= QString(), bool play= true);
    /**
     * @brief updateUi
     */
    void updateUi(bool isGM);
    /**
     * @brief addActionsIntoMenu
     * @param menu
     */
    void addActionsIntoMenu(QMenu* menu);

    /**
     * @brief readM3uPlayList
     * @param filepath
     */
    void readM3uPlayList(QString filepath);
    /**
     * @brief addSongIntoModel
     * @param str
     */
    void addSongIntoModel(QString str);

    const MusicModel* model() const;
    void setPlayingMode(PlayerWidget::PlayingMode mode);
public slots:
    // ********************** Slots from network *************************** //
    /**
     * @brief playSong
     * @param pos
     */
    void playSong(qint64 pos);
    /**
     * @brief stop
     */
    void stop();
    /**
     * @brief pause
     */
    void pause();
    /**
     * @brief setPositionAt
     * @param pos
     */
    void setPositionAt(qint64 pos);
    /**
     * @brief setSourceSong
     * @param file
     */
    void setSourceSong(QString file);
    /**
     * @brief triggeredPlayingModeAction
     */
    void triggeredPlayingModeAction();
    /**
     * @brief errorOccurs
     */
    void errorOccurs(QMediaPlayer::Error);
    void playSelectedSong();

protected:
    /**
     * @brief contextMenuEvent
     * @param ev
     */
    void contextMenuEvent(QContextMenuEvent* ev);
    void dropEvent(QDropEvent* event);
protected slots:
    /**
     * @brief setCurrentFile
     * @param current
     * @param previous
     */
    virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    /**
     * @brief setupUi
     */
    void setupUi();

private slots:
    /**
     * @brief savePlaylist
     */
    void savePlaylist();
    /**
     * @brief setTime
     * @param time
     */
    void setTime(int time);
    /**
     * @brief positionChanged
     * @param time
     */
    void positionChanged(qint64 time);
    /**
     * @brief setDuration
     * @param duration
     */
    void setDuration(qint64 duration);
    void updateIcon();
    /**
     * @brief sourceChanged
     * @param media
     */
    void sourceChanged(const QMediaContent& media);

    /**
     * @brief playerStatusChanged
     * @param newState
     */
    void playerStatusChanged(QMediaPlayer::State newState);
    /**
     * @brief mediaStatusChanged
     * @param status
     */
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    /**
     * @brief  slot which manage the click on add song button
     * @param QModelIndex
     */
    void startMediaByModelIndex(QModelIndex p);
    /**
     * @brief  slot which manage the click on remove song button
     */
    void removeFile();
    /**
     * @brief  slot which manage the click on remove song button
     */
    void addFiles();
    /**
     * @brief openPlayList
     */
    void openPlayList();
    /**
     * @brief saveVolumeValue
     */
    void saveVolumeValue(int);
    /**
     * @brief removeAll
     */
    void removeAll();
    /**
     * @brief changeDirectory
     */
    void changeDirectory();
    /**
     * @brief labelTextChanged
     */
    void labelTextChanged();
    /**
     * @brief findNext
     */
    void findNext();
    /**
     * @brief loadPlayList
     */
    void loadPlayList();
    /**
     * @brief askToDeleteAll
     * @return
     */
    bool askToDeleteAll();
    /**
     * @brief openStream
     */
    void openStream();
signals:
    /**
     * @brief positionChanged
     */
    void positionChanged(int);
    /**
     * @brief volumeChanged
     */
    void volumeChanged(int);
    /**
     * @brief askNext
     */
    void askNext();
    /**
     * @brief askPrevious
     */
    void askPrevious();
    // ****************** For NetWork ******** //
    /**
     * @brief playerStopped
     */
    void playerStopped(int);
    /**
     * @brief playerIsPaused
     */
    void playerIsPaused(int);
    /**
     * @brief playerIsPlaying
     */
    void playerIsPlaying(int, qint64);
    /**
     * @brief newSongPlayed
     */
    void newSongPlayed(int, QString);
    /**
     * @brief playerPositionChanged
     */
    void playerPositionChanged(int, qint64);
    void newPlaylistLoaded(QString path);

private:
    QSlider* m_volume;
    QSlider* m_seek;
    QMediaPlayer m_player;
    QMediaContent* m_content;
    QAction* m_playAct;
    QAction* m_stopAct;
    QAction* m_pauseAct;
    QAction* m_uniqueAct;
    QAction* m_repeatAct;
    QAction* m_shuffleAct;
    QAction* m_changeDirectoryAct;
    QAction* m_volumeMutedAct;
    QAction* m_loadTableTopAudioPlayListAct;
    PreferencesManager* m_preferences;
    MusicModel* m_model;
    QAction* m_addAction; //!< @brief add song action
    QAction* m_addStreamAction;
    QAction* m_deleteAction; //!< @brief remove song action
    QAction* m_openPlayList;
    QAction* m_savePlayList;
    QAction* m_clearList;
    PlayingMode m_playingMode;
    QMediaContent* m_currentContent;
    QString m_audioFileFilter;

    int m_id;
    qint64 m_time;
    std::mt19937 m_rng;

    Ui::AudioWidgetUI* m_ui;
    bool m_isGM;
};
#endif
