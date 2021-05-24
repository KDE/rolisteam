/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include "characterstatemodel.h"
#include "network/networkmessagewriter.h"
#include "preferences/preferencesmanager.h"

#include "data/character.h"
#include <QBuffer>
#include <QColor>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

CharacterStateInfo makeStateInfo(CharacterState* state, bool remote)
{
    CharacterStateInfo a;
    a.state= state;
    a.remote= remote;
    return a;
}

CharacterStateModel::CharacterStateModel(QObject* parent) : QAbstractListModel(parent)
{
    m_header << tr("Label") << tr("Color") << tr("Image");
}

CharacterStateModel::~CharacterStateModel() {}

QVariant CharacterStateModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return {};

    QVariant var;
    CharacterState* state= m_stateList[static_cast<std::size_t>(index.row())].state;
    bool remote= m_stateList[static_cast<std::size_t>(index.row())].remote;

    if((role == Qt::DisplayRole || role == Qt::EditRole) || (role == Qt::BackgroundRole && index.column() == 1))
    {
        if(role != Qt::DisplayRole || index.column() != 1)
            role= index.column() + Qt::UserRole + 1;
    }

    switch(role)
    {
    case LABEL:
        var= state->getLabel();
        break;
    case ID:
        var= state->id();
        break;
    case COLOR:
        var= state->getColor();
        break;
    case PICTURE:
        var= state->getImage();
        break;
    case REMOTE:
        var= remote;
        break;
    }
    return var;
}
int CharacterStateModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return static_cast<int>(m_stateList.size());
    return 0;
}
int CharacterStateModel::columnCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return m_header.size();
    return 0;
}
QVariant CharacterStateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::DisplayRole == role)
    {
        if(orientation == Qt::Horizontal)
        {
            return m_header.at(section);
        }
    }
    return QVariant();
}
void CharacterStateModel::setStates(QList<CharacterState*>* lst)
{
    beginResetModel();
    std::transform(lst->begin(), lst->end(), std::back_inserter(m_stateList),
                   [](CharacterState* state) -> CharacterStateInfo {
                       CharacterStateInfo a= makeStateInfo(state, false);
                       // TODO c++14 list initializer
                       return a;
                   });
    endResetModel();
}
void CharacterStateModel::appendState()
{
    addState(new CharacterState());
}
void CharacterStateModel::preferencesHasChanged(const QString& pref)
{
    if(pref == "isPlayer")
    {
        // m_isGM= !PreferencesManager::getInstance()->value(pref, true).toBool();
    }
}
NetWorkReceiver::SendType CharacterStateModel::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::AllExceptSender;

    if(nullptr == msg)
        return NetWorkReceiver::NONE;

    switch(msg->action())
    {
    case NetMsg::addCharacterState:
        processAddState(msg);
        break;
    case NetMsg::removeCharacterState:
        processRemoveState(msg);
        break;
    case NetMsg::moveCharacterState:
        processMoveState(msg);
        break;
    case NetMsg::CharactereStateModel:
        processModelState(msg);
        break;
    default:
        break;
    }

    return type;
}

void CharacterStateModel::processModelState(NetworkMessageReader* msg)
{
    // TODO
}

void CharacterStateModel::addState(CharacterState* state)
{
    auto pos= static_cast<int>(m_stateList.size());
    beginInsertRows(QModelIndex(), pos, pos);
    CharacterStateInfo a= makeStateInfo(state, false);
    m_stateList.push_back(a);
    endInsertRows();
}
Qt::ItemFlags CharacterStateModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}
bool CharacterStateModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool result= false;
    if(index.isValid())
    {
        auto stateInfo= m_stateList.at(index.row());
        if(role == Qt::EditRole)
        {
            switch(index.column())
            {
            case LABEL:
                stateInfo.state->setLabel(value.toString());
                result= true;
                break;
            case COLOR:
                stateInfo.state->setColor(value.value<QColor>());
                result= true;
                break;
            case PICTURE:
                stateInfo.state->setImage(value.value<QPixmap>());
                result= true;
                break;
            }
        }
        if(result) // isGM
        {
            NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addCharacterState);
            msg.int64(index.row());
            msg.string32(stateInfo.state->getLabel());
            msg.rgb(stateInfo.state->getColor().rgb());
            msg.uint8(stateInfo.state->hasImage());
            if(stateInfo.state->hasImage())
            {
                auto img= stateInfo.state->getImage();
                QByteArray array;
                QBuffer buffer(&array);
                img.save(&buffer, "PNG");
                msg.byteArray32(array);
            }
            msg.sendToServer();
        }
    }
    return result;
}
QList<CharacterState*> CharacterStateModel::getCharacterStates()
{
    QList<CharacterState*> data;
    std::transform(m_stateList.begin(), m_stateList.end(), std::back_inserter(data),
                   [](const CharacterStateInfo& info) { return info.state; });
    return data;
}
void CharacterStateModel::deleteState(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_stateList.erase(m_stateList.begin() + index.row());
    endRemoveRows();

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::removeCharacterState);
    msg.int64(index.row());
    msg.sendToServer();
}
void CharacterStateModel::upState(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    if(index.row() == 0)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), index.row() - 1))
    {
        std::iter_swap(m_stateList.begin() + index.row(), m_stateList.begin() + index.row() - 1);
        moveState(index.row(), index.row() - 1);
        endMoveRows();
    }
}

void CharacterStateModel::downState(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row() == m_stateList.size() - 1)
        return;

    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), index.row() + 2))
    {
        std::iter_swap(m_stateList.begin() + index.row(), m_stateList.begin() + index.row() + 1);
        moveState(index.row(), index.row() + 1);
        endMoveRows();
    }
}

void CharacterStateModel::topState(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row() == 0)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), 0))
    {
        auto it= m_stateList.begin() + index.row();
        auto val= *it;
        m_stateList.erase(it);
        m_stateList.insert(m_stateList.begin(), val);
        moveState(index.row(), 0);
        endMoveRows();
    }
}

void CharacterStateModel::bottomState(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    auto size= static_cast<int>(m_stateList.size());
    if(index.row() == size - 1)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), size))
    {
        auto it= m_stateList.begin() + index.row();
        auto val= *it;
        m_stateList.erase(it);
        m_stateList.insert(m_stateList.end(), val);
        moveState(index.row(), size);
        endMoveRows();
    }
}

void CharacterStateModel::clear()
{
    beginResetModel();
    m_stateList.clear();
    endResetModel();
}
void CharacterStateModel::processAddState(NetworkMessageReader* msg)
{
    CharacterState* state= new CharacterState();

    quint64 id= msg->uint64();
    state->setLabel(msg->string32());
    state->setColor(msg->rgb());
    bool hasImage= static_cast<bool>(msg->uint8());
    if(hasImage)
    {
        QByteArray array= msg->byteArray32();
        QPixmap pix;
        pix.loadFromData(array);
        state->setImage(pix);
    }
    m_stateList.push_back(makeStateInfo(state, true));
}
void CharacterStateModel::processMoveState(NetworkMessageReader* msg)
{
    int from= msg->int64();
    int to= msg->int64();

    if((from >= 0) && (from < m_stateList.size()))
    {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        std::iter_swap(m_stateList.begin() + to, m_stateList.begin() + from);
        endMoveRows();
    }
}
void CharacterStateModel::processRemoveState(NetworkMessageReader* msg)
{
    int pos= static_cast<int>(msg->int64());
    if(m_stateList.size() < pos)
        return;

    beginRemoveRows(QModelIndex(), pos, pos);
    m_stateList.erase(m_stateList.begin() + pos);
    endRemoveRows();
}

void CharacterStateModel::sendOffAllCharacterState()
{
    /*  for(auto& state : *m_stateList)
      {
          NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addState);
          msg.uint64(m_stateList->indexOf(state));
          msg.string32(state->getLabel());
          msg.rgb(state->getColor().rgb());
          if(state->hasImage())
          {
              msg.uint8(static_cast<quint8>(true));

              QByteArray array;
              QBuffer buffer(&array);
              if(!state->getPixmap()->save(&buffer, "PNG"))
              {
                  qWarning("error during encoding png");
              }
              msg.byteArray32(array);
          }
          else
          {
              msg.uint8(static_cast<quint8>(false));
          }
          msg.sendToServer();
      }*/
}
void CharacterStateModel::moveState(int from, int to)
{
    // if(m_isGM)
    {
        NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::moveCharacterState);
        msg.int64(from);
        msg.int64(to);
        msg.sendToServer();
    }
}
void CharacterStateModel::load(const QJsonObject& obj)
{
    QJsonArray states= obj["states"].toArray();

    for(auto stateRef : states)
    {
        auto state= stateRef.toObject();
        auto da= new CharacterState();
        da->setLabel(state["label"].toString());
        da->setIsLocal(state["local"].toBool());
        auto base64= state["image"].toString();
        if(!base64.isEmpty())
        {
            QByteArray array= QByteArray::fromBase64(base64.toUtf8());
            QPixmap pix;
            pix.loadFromData(array);
            da->setImage(pix);
        }
        QColor col;
        col.setNamedColor(state["color"].toString());
        da->setColor(col);
        addState(da);
    }
}

void CharacterStateModel::save(QJsonObject& obj)
{
    QJsonArray states;
    for(auto& stateInfo : m_stateList)
    {
        if(stateInfo.remote)
            continue;
        QJsonObject stateObj;
        stateObj["label"]= stateInfo.state->getLabel();
        stateObj["color"]= stateInfo.state->getColor().name();
        QPixmap pix= stateInfo.state->getImage();
        if(!pix.isNull())
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix.save(&buffer, "PNG");
            stateObj["image"]= QString(buffer.data().toBase64());
        }
        stateObj["local"]= stateInfo.state->isLocal();
        states.append(stateObj);
    }
    obj["states"]= states;
}
