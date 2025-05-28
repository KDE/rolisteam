/***************************************************************************
 *	Copyright (C) 2014 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "model/musicmodel.h"

#include <QFont>
#include <QMimeData>
#include <QNetworkRequest>
#include <QSet>
#include <QUrl>

#include "preferences/preferencesmanager.h"
#include "worker/iohelper.h"

MusicModel::MusicModel(PreferencesManager* pref, QObject* parent)
    : QAbstractListModel(parent), m_header({tr("Title")}), m_ctrl(pref)
{
}

int MusicModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return m_data.size();
}

QVariant MusicModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return {};

    if(Qt::DisplayRole == role)
    {
        return m_header[section];
    }

    return {};
}

namespace
{
QFont boldFont()
{
    QFont font;
    font.setBold(true);
    return font;
}

/*QString normalizeUrl(const QUrl& url)
{
    if(url.isLocalFile() || url.host().contains("tabletopaudio.com") == false)
        return url.fileName();

    QString str= url.toString();
    str= str.right(str.size() - (str.lastIndexOf("=") + 1));
    return str.replace(".mp3", "").replace("_", " ");
}*/
} // namespace

QVariant MusicModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || m_data.size() <= index.row())
        return {};

    // Break early if role is not Diplay or Font.
    QSet<int> set({Qt::DisplayRole, Qt::FontRole, Qt::ToolTipRole, TITLE, URL});
    if(!set.contains(role))
        return {};

    auto url= m_data.at(index.row());
    QVariant var;
    // qDebug() << "data song:" << m_currentSong << url;
    switch(role)
    {
    case Qt::DisplayRole:
    case TITLE:
        var= url.fileName();
        break;
    case Qt::ToolTipRole:
    case URL:
        var= url;
        break;
    case Qt::FontRole:
        if(url.toString() == m_currentSong.toString())
        {
            var= QVariant(boldFont());
        }
        break;
    default:
        break;
    }

    return var;
}

void MusicModel::addSong(const QList<QUrl>& list)
{
    if(list.isEmpty())
        return;

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + list.size() - 1);
    m_data.reserve(m_data.size() + list.size());

    for(auto& url : list)
    {
        m_data.append(url);
    }
    endInsertRows();
}
void MusicModel::insertSong(int i, QUrl url)
{
    if(!url.isValid())
        return;
    if(0 > i)
    {
        i= m_data.size();
    }

    beginInsertRows(QModelIndex(), i, i);
    m_data.insert(i, url);
    endInsertRows();
}

QUrl MusicModel::getMediaByModelIndex(const QModelIndex& index) const
{
    return m_data.at(index.row());
}

void MusicModel::removeAll()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
void MusicModel::removeSong(const QModelIndexList& list)
{
    if(list.isEmpty())
        return;

    QModelIndex first= list[0];
    QModelIndex end= list.last();
    beginRemoveRows(first.parent(), first.row(), end.row());
    for(int i= list.size() - 1; i >= 0; --i)
    {
        QModelIndex index= list[i];
        m_data.removeAt(index.row());
    }
    endRemoveRows();
}
void MusicModel::setCurrentSong(const QModelIndex& p)
{
    m_currentSong= data(p, URL).toUrl();
    emit dataChanged(p, p);
}

QStringList MusicModel::mimeTypes() const
{
    return {"text/uri-list"};
}

int MusicModel::indexOfCurrent() const
{
    return m_data.indexOf(m_currentSong);
}

const QList<QUrl>& MusicModel::urls() const
{
    return m_data;
}

Qt::DropActions MusicModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
bool MusicModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int, const QModelIndex&)
{
    if(action == Qt::IgnoreAction)
        return true;

    if(!data->hasUrls())
        return false;

    QList<QUrl> list= data->urls();
    for(auto const& url : std::as_const(list))
    {
        QString str= url.toLocalFile();
        if(str.endsWith(".m3u"))
        {
            addSong(IOHelper::readM3uPlayList(str));
        }
        else
        {
            QStringList filters
                = m_ctrl->value("AudioFileFilter", "*.wav *.mp2 *.mp3 *.mpc *.ogg *.flac").toString().split(' ');
            for(auto filter : std::as_const(filters))
            {
                filter.replace("*", "");
                if(str.endsWith(filter))
                {
                    insertSong(row, str);
                }
            }
        }
    }

    return true;
}
Qt::ItemFlags MusicModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags= QAbstractListModel::flags(index);
    if(!index.isValid())
        defaultFlags|= Qt::ItemIsDropEnabled;

    return defaultFlags;
}
