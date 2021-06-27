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
#ifndef SINGLECONTENTTYPEMODEL_H
#define SINGLECONTENTTYPEMODEL_H

#include <QSortFilterProxyModel>

#include "media/mediatype.h"


class MediaControllerBase;
class SingleContentTypeModel : public QSortFilterProxyModel
{
public:
    SingleContentTypeModel(Core::ContentType type, QObject* parent = nullptr);
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent);


    Core::ContentType type() const;
    bool contains(const QString& uuid) const;
    MediaControllerBase *controller(const QString &uuid) const;
private:
    Core::ContentType m_type;
};

#endif // SINGLECONTENTTYPEMODEL_H
