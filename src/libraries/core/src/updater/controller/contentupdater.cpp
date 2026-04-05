#include "updater/controller/contentupdater.h"

#include "media/mediafactory.h"
#include "worker/messagehelper.h"
#include <QSet>
#include <set>

ContentUpdater::ContentUpdater(ContentController* ctrl, QObject* parent) : NetWorkReceiver{parent}, m_ctrl{ctrl}
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::MediaCategory, this);
}

NetWorkReceiver::SendType ContentUpdater::processMessage(NetworkMessageReader* msg)
{

    NetWorkReceiver::SendType result= NetWorkReceiver::NONE;
    std::set<NetMsg::Action> actions({NetMsg::AddMedia, NetMsg::UpdateMediaProperty, NetMsg::CloseMedia,
                                      NetMsg::AddSubImage, NetMsg::RemoveSubImage});

    auto contentModel= m_ctrl->contentModel();
    if(!contentModel || !msg || actions.find(msg->action()) == actions.end())
        return result;

    QSet<NetMsg::Action> subActions{NetMsg::UpdateMediaProperty, NetMsg::AddSubImage, NetMsg::RemoveSubImage};

    if(msg->action() == NetMsg::CloseMedia)
    {
        Q_UNUSED(static_cast<Core::ContentType>(msg->uint8()));
        auto id= MessageHelper::readMediaId(msg);
        contentModel->removeMedia(id);
    }
    else if(msg->action() == NetMsg::AddMedia)
    {
        auto type= static_cast<Core::ContentType>(msg->uint8());
        auto media= Media::MediaFactory::createRemoteMedia(type, msg, m_ctrl->localColor(), m_ctrl->localIsGM());
        contentModel->appendMedia(media);
    }
    else if(subActions.contains(msg->action()))
    {
        auto type= static_cast<Core::ContentType>(msg->uint8());
        auto updater= m_ctrl->mediaUpdaters(type);
        if(updater)
        {
            updater->processMessage(msg);
        }
        else
        {
            QString mediaId= msg->string8();
        }
    }
    return result;
}
