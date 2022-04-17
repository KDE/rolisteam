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
#ifndef MINDMAP_FILESERIALIZER_H
#define MINDMAP_FILESERIALIZER_H

#include <QObject>
#include "mindmap/mindmap_global.h"
namespace mindmap
{

class BoxModel;
class LinkModel;
class MINDMAP_EXPORT FileSerializer : public QObject
{
    Q_OBJECT
public:
    FileSerializer();

    static bool readTextFile(BoxModel* nodeModel, LinkModel* linkModel, const QString& filepath);
    static bool readFile(BoxModel* nodeModel, LinkModel* linkModel, const QString& filepath);
    static bool writeFile(BoxModel* nodeModel, LinkModel* linkModel, const QString& filepath);
};
} // namespace mindmap

#endif // MINDMAP_FILESERIALIZER_H
