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
#include "vmapitemfactory.h"

#include "vmap/controller/characteritemcontroller.h"
#include "vmap/controller/ellipsecontroller.h"
#include "vmap/controller/imagecontroller.h"
#include "vmap/controller/linecontroller.h"
#include "vmap/controller/pathcontroller.h"
#include "vmap/controller/rectcontroller.h"
#include "vmap/controller/textcontroller.h"

#include "network/networkmessagereader.h"
#include "worker/messagehelper.h"

vmap::VisualItemController* vmap::VmapItemFactory::createVMapItem(VectorialMapController* mapCtrl,
                                                                  Core::SelectableTool tool,
                                                                  const std::map<QString, QVariant>& param)
{
    vmap::VisualItemController* ctrl= nullptr;
    switch(tool)
    {
    case Core::SelectableTool::LINE:
        ctrl= new LineController(param, mapCtrl);
        break;
    case Core::SelectableTool::FILLEDELLIPSE:
    case Core::SelectableTool::EMPTYELLIPSE:
        ctrl= new vmap::EllipseController(param, mapCtrl);
        break;
    case Core::SelectableTool::EMPTYRECT:
    case Core::SelectableTool::FILLRECT:
        ctrl= new vmap::RectController(param, mapCtrl);
        break;
    case Core::SelectableTool::TEXT:
    case Core::SelectableTool::TEXTBORDER:
        ctrl= new vmap::TextController(param, mapCtrl);
        break;
    case Core::SelectableTool::NonPlayableCharacter:
    case Core::SelectableTool::PlayableCharacter:
        ctrl= new vmap::CharacterItemController(param, mapCtrl);
        break;
    case Core::SelectableTool::IMAGE:
        ctrl= new vmap::ImageController(param, mapCtrl);
        break;
    case Core::SelectableTool::PEN:
    case Core::SelectableTool::PATH:
        ctrl= new vmap::PathController(param, mapCtrl);
        break;
    case Core::SelectableTool::HANDLER:
    case Core::SelectableTool::RULE:
    case Core::SelectableTool::ANCHOR:
    case Core::SelectableTool::PIPETTE:
    case Core::SelectableTool::BUCKET:
    case Core::SelectableTool::HIGHLIGHTER:
        break;
    }
    Q_ASSERT(ctrl != nullptr);
    return ctrl;
}

vmap::VisualItemController* vmap::VmapItemFactory::createRemoteVMapItem(VectorialMapController* mapCtrl,
                                                                        NetworkMessageReader* msg)
{
    auto itemType= static_cast<vmap::VisualItemController::ItemType>(msg->uint8());

    vmap::VisualItemController* ctrl= nullptr;
    switch(itemType)
    {
    case vmap::VisualItemController::LINE:
    {
        auto param= MessageHelper::readLine(msg);
        ctrl= new LineController(param, mapCtrl);
    }
    break;
    case vmap::VisualItemController::ELLIPSE:
    {
        auto param= MessageHelper::readEllipse(msg);
        ctrl= new vmap::EllipseController(param, mapCtrl);
    }
    break;
    case vmap::VisualItemController::RECT:
    {
        auto param= MessageHelper::readRect(msg);
        ctrl= new vmap::RectController(param, mapCtrl);
    }
    break;
    case vmap::VisualItemController::TEXT:
    {
        auto param= MessageHelper::readText(msg);
        ctrl= new vmap::TextController(param, mapCtrl);
    }
    break;
    case vmap::VisualItemController::CHARACTER:
    {
        auto param= MessageHelper::readCharacter(msg);
        ctrl= new vmap::CharacterItemController(param, mapCtrl);
    }
    break;
    case vmap::VisualItemController::IMAGE:
    {
        auto param= MessageHelper::readImage(msg);
        ctrl= new vmap::ImageController(param, mapCtrl);
    }
    break;
    case vmap::VisualItemController::PATH:
    {
        auto param= MessageHelper::readPath(msg);
        ctrl= new vmap::PathController(param, mapCtrl);
    }
    break;
    default:
        break;
    }
    Q_ASSERT(ctrl != nullptr);
    ctrl->setRemote(true);
    return ctrl;
}
