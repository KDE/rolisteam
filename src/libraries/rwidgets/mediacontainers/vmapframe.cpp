/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                    *
 *                                                                          *
 *   rolisteam is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QUuid>
#include <QVBoxLayout>

#include "vmapframe.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

QString visibilityText(Core::VisibilityMode vis)
{
    QStringList visibilityData;
    visibilityData << QObject::tr("Hidden") << QObject::tr("Fog Of War") << QObject::tr("All visible");

    if(vis < visibilityData.size())
    {
        return visibilityData.at(vis);
    }
    return {};
}

QString permissionModeText(Core::PermissionMode mode)
{
    QStringList permissionData;
    permissionData << QObject::tr("No Right") << QObject::tr("His character") << QObject::tr("All Permissions");
    return permissionData.at(mode);
}

QString layerText(Core::Layer mode)
{
    QString str;
    switch(mode)
    {
    case Core::Layer::GROUND:
        str= QObject::tr("Ground");
        break;
    case Core::Layer::OBJECT:
        str= QObject::tr("Object");
        break;
    case Core::Layer::CHARACTER_LAYER:
        str= QObject::tr("Character");
        break;
    case Core::Layer::FOG:
        str= QObject::tr("Fog Layer");
        break;
    case Core::Layer::GRIDLAYER:
        str= QObject::tr("Grid Layer");
        break;
    case Core::Layer::NONE:
        str= QObject::tr("No Layer");
        break;
    }
    return str;
}

VMapFrame::VMapFrame(VectorialMapController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::VMapContainer, parent)
    , m_ctrl(ctrl)
    , m_vmap(new VMap(ctrl))
    , m_graphicView(new RGraphicsView(ctrl))
    , m_toolbox(new VToolsBar(ctrl))
    , m_toolbar(new VmapToolBar(ctrl))
{
    setupUi();

    setObjectName("VMapFrame");
    setWindowIcon(QIcon::fromTheme("vmap"));
    m_graphicView->setScene(m_vmap.get());

    auto func= [this]() {
        setWindowTitle(tr("%1 - visibility: %2 - permission: %3 - layer: %4")
                           .arg(m_ctrl->name(), visibilityText(m_ctrl->visibility()),
                                permissionModeText(m_ctrl->permission()), layerText(m_ctrl->layer())));
    };

    connect(m_ctrl, &VectorialMapController::nameChanged, this, func);
    connect(m_ctrl, &VectorialMapController::visibilityChanged, this, func);
    connect(m_ctrl, &VectorialMapController::layerChanged, this, func);
    connect(m_ctrl, &VectorialMapController::permissionChanged, this, func);

    func();
}

VMapFrame::~VMapFrame() {}

void VMapFrame::setupUi()
{
    auto wid= new QWidget(this);
    auto layout= new QVBoxLayout(wid);
    layout->addWidget(m_toolbar.get());

    auto horizontal= new QSplitter(Qt::Horizontal, this);
    layout->addWidget(horizontal);

    horizontal->addWidget(m_toolbox.get());
    horizontal->addWidget(m_graphicView.get());

    setWidget(wid);
}

bool VMapFrame::defineMenu(QMenu* /*menu*/)
{
    return false;
}

bool VMapFrame::openFile(const QString& filepath)
{
    if(filepath.isEmpty())
        return false;

    QFile input(filepath);
    if(!input.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&input);
    in.setVersion(QDataStream::Qt_5_7);
    return true;
}

void VMapFrame::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
    case Qt::Key_Delete:
        /// @todo remove selected item
        break;
    case Qt::Key_Escape:
        // m_toolsbar->setCurrentTool(VToolsBar::HANDLER);
        emit defineCurrentTool(Core::HANDLER);
        break;
    default:
        MediaContainer::keyPressEvent(event);
    }
}
