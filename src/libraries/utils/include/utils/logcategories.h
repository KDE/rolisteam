/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef LOGCATEGORIES_H
#define LOGCATEGORIES_H
#include "utils/utils_global.h"

#include <QLoggingCategory>

namespace logCategory
{
static const QLoggingCategory map("map");
static const QLoggingCategory campaign("campaign");
static const QLoggingCategory pdf("pdf");
static const QLoggingCategory img("image");
static const QLoggingCategory mindmap("mindmap");
static const QLoggingCategory im("instantmessaging");
static const QLoggingCategory sheet("charactersheet");
static const QLoggingCategory server("server");
static const QLoggingCategory rcse("rcse");
static const QLoggingCategory rolisteam("rolisteam");
static const QLoggingCategory network("network");
}; // namespace logCategory

#endif // LOGCATEGORIES_H
