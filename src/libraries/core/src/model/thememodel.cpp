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
#include "model/thememodel.h"

#include "data/rolisteamtheme.h"

ThemeModel::ThemeModel(QObject* parent) : QAbstractListModel(parent) {}

QVariant ThemeModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

ThemeModel::~ThemeModel()= default;

int ThemeModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_themeList.size());
}

int ThemeModel::indexOf(RolisteamTheme* theme) const
{
    auto it= std::find_if(std::begin(m_themeList), std::end(m_themeList),
                          [theme](const std::unique_ptr<RolisteamTheme>& itTheme) { return itTheme.get() == theme; });

    if(it == std::end(m_themeList))
        return -1;
    return std::distance(std::begin(m_themeList), it);
}

int ThemeModel::indexOf(const QString& uuid) const
{
    auto it= std::find_if(std::begin(m_themeList), std::end(m_themeList),
                          [uuid](const std::unique_ptr<RolisteamTheme>& itTheme) { return itTheme->uuid() == uuid; });

    if(it == std::end(m_themeList))
        return -1;
    return std::distance(std::begin(m_themeList), it);
}

QVariant ThemeModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
        role= Name;

    const auto& theme= m_themeList.at(static_cast<std::size_t>(index.row()));

    if(role == Qt::DecorationRole && !theme->isRemovable())
        return QIcon::fromTheme("lock.png");

    QVariant var;
    switch(role)
    {
    case Name:
        var= QVariant::fromValue(theme->getName());
        break;
    }
    return var;
}

void ThemeModel::removeTheme(int pos)
{

    auto idx= static_cast<std::size_t>(pos);
    if(m_themeList.size() <= idx)
        return;

    beginRemoveRows(QModelIndex(), pos, pos);
    m_themeList.erase(m_themeList.begin() + pos);
    endRemoveRows();
}

void ThemeModel::addTheme(RolisteamTheme* theme)
{
    auto size= static_cast<int>(m_themeList.size());
    beginInsertRows(QModelIndex(), size, size);
    std::unique_ptr<RolisteamTheme> ptr(theme);
    m_themeList.push_back(std::move(ptr));
    endInsertRows();
}
void ThemeModel::clear()
{
    beginResetModel();
    m_themeList.clear();
    endResetModel();
}
const std::vector<std::unique_ptr<RolisteamTheme>>& ThemeModel::themes() const
{
    return m_themeList;
}
QString ThemeModel::name(int themePos) const
{
    auto idx= static_cast<std::size_t>(themePos);

    if(idx >= m_themeList.size())
        return {};

    const auto& it= m_themeList.at(idx);
    return it->getName();
}
RolisteamTheme* ThemeModel::theme(int pos) const
{
    auto idx= static_cast<std::size_t>(pos);

    if(idx >= m_themeList.size())
        return nullptr;

    const auto& it= m_themeList.at(idx);
    return it.get();
}
RolisteamTheme* ThemeModel::theme(const QString& uuid) const
{
    qDebug() << "CAMPAIGN::" << m_themeList.size();
    auto it= std::find_if(std::begin(m_themeList), std::end(m_themeList),
                          [uuid](const std::unique_ptr<RolisteamTheme>& theme)
                          {
                              qDebug() << "CAMPAIGN:: theme" << theme->uuid();
                              return uuid == theme->uuid();
                          });

    if(it == std::end(m_themeList))
        return nullptr;

    return (*it).get();
}
