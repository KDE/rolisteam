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
#include "vectorialmapmediacontroller.h"

#include <QUndoStack>
#include <map>

#include "controller/networkcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/receiveevent.h"
#include "undoCmd/removemediacontrollercommand.h"
#include "updater/vmapupdater.h"
#include "vmap/vmapframe.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

VectorialMapController* findActive(const std::vector<std::unique_ptr<VectorialMapController>>& vmaps)
{
    auto it= std::find_if(vmaps.begin(), vmaps.end(),
                          [](const std::unique_ptr<VectorialMapController>& ctrl) { return ctrl->isActive(); });
    if(vmaps.end() == it)
        return nullptr;
    return (*it).get();
}

VectorialMapController* findMap(const std::vector<std::unique_ptr<VectorialMapController>>& vmaps, const QString& id)
{
    auto it= std::find_if(vmaps.begin(), vmaps.end(),
                          [id](const std::unique_ptr<VectorialMapController>& ctrl) { return ctrl->uuid() == id; });
    if(vmaps.end() == it)
        return nullptr;
    return (*it).get();
}

VectorialMapMediaController::VectorialMapMediaController(ContentModel* contentModel)
    : MediaManagerBase(Core::ContentType::VECTORIALMAP, contentModel), m_updater(new VMapUpdater)
{
    /*auto func= [this]() {
        std::for_each(m_vmaps.begin(), m_vmaps.end(), [this](const std::unique_ptr<VectorialMapController>& ctrl) {
            ctrl->setLocalGM(m_networkCtrl->isGM());
        });
    };

    connect(m_networkCtrl, &NetworkController::isGMChanged, this, func);
    func();*/
}

VectorialMapController* VectorialMapMediaController::currentVMap() const
{
    return findActive(m_vmaps);
}

VectorialMapMediaController::~VectorialMapMediaController()= default;

void VectorialMapMediaController::registerNetworkReceiver()
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

/*CleverURI* findMedia(QString uuid, const std::vector<CleverURI*>& media)
{
    return nullptr;
}*/

NetWorkReceiver::SendType VectorialMapMediaController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    qDebug() << "ProcessMessage";
    if(msg->action() == NetMsg::AddMedia && msg->category() == NetMsg::MediaCategory)
    {
        auto map= addVectorialMapController("", QHash<QString, QVariant>());
        MessageHelper::readVectorialMapData(msg, map);
    }
    else if(msg->action() == NetMsg::CloseMedia && msg->category() == NetMsg::MediaCategory)
    {
        QString vmapId= msg->string8();
        closeMedia(vmapId);
    }
    else if(msg->action() == NetMsg::UpdateMediaProperty && msg->category() == NetMsg::MediaCategory)
    {
        QString vmapId= msg->string8();
        auto map= findMap(m_vmaps, vmapId);
        m_updater->updateVMapProperty(msg, map);
    }
    else if(msg->category() == NetMsg::VMapCategory)
    {
        QString vmapId= msg->string8();
        auto map= findMap(m_vmaps, vmapId);
        if(nullptr == map)
            return type;
        type= map->processMessage(msg);
    }
    return type;
}

int VectorialMapMediaController::managerCount() const
{
    return static_cast<int>(m_vmaps.size());
}

Core::SelectableTool VectorialMapMediaController::tool() const
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return Core::HANDLER;

    return ctrl->tool();
}

void VectorialMapMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_vmaps.begin(), m_vmaps.end(),
                            [id](const std::unique_ptr<VectorialMapController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_vmaps.end())
        return;
    (*it)->aboutToClose();
    m_vmaps.erase(it);
}

bool VectorialMapMediaController::openMedia(const QString& uuid, const std::map<QString, QVariant>& args)
{
    if(args.empty())
        return false;

    QHash<QString, QVariant> params(args.begin(), args.end());

    // qDebug() << "openMedia vectorialmanl" << params;

    auto vmapCtrl= addVectorialMapController(uuid, params);
    initializeOwnedVMap(vmapCtrl);
    return true;
}

void VectorialMapMediaController::initializeOwnedVMap(VectorialMapController* ctrl)
{
    MessageHelper::sendOffVMap(ctrl);
    m_updater->addController(ctrl);
}

VectorialMapController* VectorialMapMediaController::addVectorialMapController(const QString& uuid,
                                                                               const QHash<QString, QVariant>& params)
{
    std::unique_ptr<VectorialMapController> vmapCtrl(new VectorialMapController(uuid));

    QByteArray serializedData= params.value(QStringLiteral("serializedData")).toByteArray();

    if(!params.isEmpty())
    {
        vmapCtrl->setPermission(params.value(QStringLiteral("permission")).value<Core::PermissionMode>());
        vmapCtrl->setName(params.value(QStringLiteral("title")).toString());
        vmapCtrl->setBackgroundColor(params.value(QStringLiteral("bgcolor")).value<QColor>());
        vmapCtrl->setGridSize(params.value(QStringLiteral("gridSize")).toInt());
        vmapCtrl->setGridPattern(params.value(QStringLiteral("gridPattern")).value<Core::GridPattern>());
        vmapCtrl->setGridColor(params.value(QStringLiteral("gridColor")).value<QColor>());
        vmapCtrl->setVisibility(params.value(QStringLiteral("visibility")).value<Core::VisibilityMode>());
        vmapCtrl->setGridScale(params.value(QStringLiteral("scale")).toDouble());
        vmapCtrl->setScaleUnit(params.value(QStringLiteral("unit")).value<Core::ScaleUnit>());
    }
    auto val= vmapCtrl.get();
    connect(vmapCtrl.get(), &VectorialMapController::activeChanged, this,
            &VectorialMapMediaController::updateProperties);
    connect(vmapCtrl.get(), &VectorialMapController::performCommand, m_undoStack, &QUndoStack::push);
    connect(vmapCtrl.get(), &VectorialMapController::toolColorChanged, this,
            &VectorialMapMediaController::toolColorChanged);
    connect(this, &VectorialMapMediaController::localIsGMChanged, vmapCtrl.get(), &VectorialMapController::setLocalGM);
    connect(vmapCtrl.get(), &VectorialMapController::closeMe, this, [this, val]() {
        if(!m_undoStack)
            return;
        m_undoStack->push(new RemoveMediaControllerCommand(val, this));
    });

    vmapCtrl->setLocalGM(localIsGM());

    if(!serializedData.isEmpty())
        IOHelper::readVectorialMapController(val, serializedData);

    m_vmaps.push_back(std::move(vmapCtrl));
    emit vmapControllerCreated(val);
    return val;
}

std::vector<VectorialMapController*> VectorialMapMediaController::controllers() const
{
    std::vector<VectorialMapController*> vec;
    std::transform(m_vmaps.begin(), m_vmaps.end(), std::back_inserter(vec),
                   [](const std::unique_ptr<VectorialMapController>& ctrl) { return ctrl.get(); });
    return vec;
}

void VectorialMapMediaController::updateProperties()
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;

    emit npcNumberChanged(ctrl->npcNumber());
    emit toolColorChanged(ctrl->toolColor());
    emit gridSizeChanged(ctrl->gridSize());
    emit gridAboveChanged(ctrl->gridAbove());
    emit gridColorChanged(ctrl->gridColor());
    emit gridScaleChanged(ctrl->gridScale());
    emit gridUnitChanged(ctrl->scaleUnit());
    emit gridVisibilityChanged(ctrl->gridVisibility());
    emit backgroundColorChanged(ctrl->backgroundColor());
    emit layerChanged(ctrl->layer());
    emit collisionChanged(ctrl->collision());
    emit characterVision(ctrl->characterVision());
    emit visibilityModeChanged(ctrl->visibility());
    emit permissionModeChanged(ctrl->permission());

    // emit opacityChanged(ctrl->);
    // emit editionModeChanged(ctrl->editionMode());
}
void VectorialMapMediaController::setNpcNumber(int number)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setNpcNumber(number);
}

void VectorialMapMediaController::setNpcName(const QString& name)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setNpcName(name);
}

void VectorialMapMediaController::setOpacity(qreal opacity)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setOpacity(opacity);
}

void VectorialMapMediaController::setPenSize(int penSize)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setPenSize(penSize);
}

void VectorialMapMediaController::setEditionMode(Core::EditionMode mode)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setEditionMode(mode);
}
void VectorialMapMediaController::setColor(const QColor& color)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setToolColor(color);
}
void VectorialMapMediaController::setTool(Core::SelectableTool tool)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setTool(tool);
}
void VectorialMapMediaController::setCharacterVision(bool b)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setCharacterVision(b);
}

void VectorialMapMediaController::setGridUnit(Core::ScaleUnit unit)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setScaleUnit(unit);
}

void VectorialMapMediaController::setGridSize(int size)
{
    qDebug() << "grid size:" << size;
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridSize(size);
}

void VectorialMapMediaController::setGridScale(double scale)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridScale(scale);
}

void VectorialMapMediaController::setVisibilityMode(Core::VisibilityMode mode)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setVisibility(mode);
}

void VectorialMapMediaController::setPermissionMode(Core::PermissionMode mode)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setPermission(mode);
}

void VectorialMapMediaController::setLayer(Core::Layer layer)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setLayer(layer);
}

void VectorialMapMediaController::setGridPattern(Core::GridPattern pattern)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridPattern(pattern);
}

void VectorialMapMediaController::setGridVisibility(bool visible)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridVisibility(visible);
}

void VectorialMapMediaController::setGridAbove(bool above)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridAbove(above);
}

void VectorialMapMediaController::setCollision(bool b)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setCollision(b);
}

void VectorialMapMediaController::showTransparentItem()
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    // TODO implement
    // ctrl->setGridScale(scale);
}
void VectorialMapMediaController::setBackgroundColor(const QColor& color)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setBackgroundColor(color);
}

void VectorialMapMediaController::addImageToMap(const QPixmap& map) {}
