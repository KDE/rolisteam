#include "updater/vmapitem/lightcontrollerupdater.h"

#include "controller/item_controllers/lightcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"

LightControllerUpdater::LightControllerUpdater(QObject* parent) : VMapItemControllerUpdater{parent} {}
void LightControllerUpdater::addItemController(vmap::VisualItemController* ctrl, bool sendOff)
{
    if(nullptr == ctrl)
        return;

    auto lightCtrl= dynamic_cast<vmap::LightController*>(ctrl);

    if(nullptr == lightCtrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(lightCtrl, &vmap::LightController::radiusChanged, this,
            [this, lightCtrl]() { sendOffVMapChanges<qreal>(lightCtrl, QStringLiteral("radius")); });

    if(!ctrl->remote() && sendOff)
    {
        if(lightCtrl->initialized())
            MessageHelper::sendOffLight(lightCtrl, lightCtrl->mapUuid());
        else
            connect(lightCtrl, &vmap::LightController::initializedChanged, this,
                    [lightCtrl]() { MessageHelper::sendOffLight(lightCtrl, lightCtrl->mapUuid()); });
    }
}

bool LightControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    auto datapos= msg->pos();

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;
    if(property == QStringLiteral("radius"))
    {
        var= QVariant::fromValue(msg->real());
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
