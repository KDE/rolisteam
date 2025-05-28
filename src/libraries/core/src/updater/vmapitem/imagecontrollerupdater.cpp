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
#include "updater/vmapitem/imagecontrollerupdater.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>

#include "controller/item_controllers/imageitemcontroller.h"
#include "network/networkmessagereader.h"
#include "worker/convertionhelper.h"
#include "worker/messagehelper.h"

ImageControllerUpdater::ImageControllerUpdater() {}

void ImageControllerUpdater::addItemController(vmap::VisualItemController* ctrl, bool sendOff)
{
    if(nullptr == ctrl)
        return;

    auto imgCtrl= dynamic_cast<vmap::ImageItemController*>(ctrl);

    if(nullptr == imgCtrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(imgCtrl, &vmap::ImageItemController::dataChanged, this,
            [this, imgCtrl]() { sendOffVMapChanges<QByteArray>(imgCtrl, QStringLiteral("data")); });
    connect(imgCtrl, &vmap::ImageItemController::rectEditFinished, this,
            [this, imgCtrl]() { sendOffVMapChanges<QRectF>(imgCtrl, QStringLiteral("rect")); });

    if(!ctrl->remote() && sendOff)
        MessageHelper::sendOffImage(imgCtrl, imgCtrl->mapUuid());
}

bool ImageControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    auto datapos= msg->pos();

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;

    if(property == QStringLiteral("data"))
    {
        var= QVariant::fromValue(msg->byteArray32());
    }
    else if(property == QStringLiteral("rect"))
    {
        auto x= msg->real();
        auto y= msg->real();
        auto w= msg->real();
        auto h= msg->real();
        var= QVariant::fromValue(QRectF(x, y, w, h));
    }
    else
    {
        msg->resetToPos(datapos);
        return VMapItemControllerUpdater::updateItemProperty(msg, ctrl);
    }

    m_updatingFromNetwork= true;
    ctrl->setNetworkUpdate(true);
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    ctrl->setNetworkUpdate(false);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}
