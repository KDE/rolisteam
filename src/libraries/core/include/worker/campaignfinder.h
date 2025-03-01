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
#ifndef CAMPAIGNFINDER_H
#define CAMPAIGNFINDER_H

#include <QObject>
#include <core_global.h>

#include <QObject>
#include <QPointer>
namespace campaign
{
class CampaignManager;
}

class CORE_EXPORT CampaignFinder
{
public:
    explicit CampaignFinder();

    static bool pathIsInCampaign(const QString& path);
    static void setManager(campaign::CampaignManager* manager);
    static QString campaignRoot();

private:
    static QPointer<campaign::CampaignManager> m_manager;
};

#endif // CAMPAIGNFINDER_H
