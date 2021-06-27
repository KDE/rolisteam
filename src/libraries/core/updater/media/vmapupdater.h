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
#ifndef VMAPUPDATER_H
#define VMAPUPDATER_H

#include "mediaupdaterinterface.h"
#include "vmap/controller/visualitemcontroller.h"

#include <QObject>

class VectorialMapController;
class VMapItemControllerUpdater;
class NetworkMessageReader;
class FilteredContentModel;
class VMapUpdater : public MediaUpdaterInterface
{
    Q_OBJECT
public:
    explicit VMapUpdater(FilteredContentModel* model, QObject* parent= nullptr);

    void addMediaController(MediaControllerBase* ctrl) override;

    bool updateVMapProperty(NetworkMessageReader* msg, VectorialMapController* ctrl);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

private:
    bool m_updatingFromNetwork= false;
    VectorialMapController* updatingCtrl= nullptr;
    std::map<vmap::VisualItemController::ItemType, std::unique_ptr<VMapItemControllerUpdater>> m_updaters;
    QPointer<FilteredContentModel> m_vmapModel;
};

#endif // VMAPUPDATER_H
