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
#include "webpagemediacontroller.h"

#include <QDebug>
#include <QString>
#include <QUrl>

#include "controller/view_controller/webpagecontroller.h"
#include "undoCmd/removemediacontrollercommand.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

WebpageMediaController::WebpageMediaController(ContentModel* contentModel)
    : MediaManagerBase(Core::ContentType::WEBVIEW, contentModel)
{
}

WebpageMediaController::~WebpageMediaController()= default;

bool WebpageMediaController::openMedia(const QString& id, const std::map<QString, QVariant>& args)
{
    if(id.isEmpty() && args.empty())
        return false;

    QByteArray serializedData;
    auto it= args.find(QStringLiteral("serializedData"));
    if(it != args.end())
        serializedData= it->second.toByteArray();

    auto state= m_localIsGM ? WebpageController::localIsGM : WebpageController::LocalIsPlayer;
    addWebpageController(
        QHash<QString, QVariant>({{"id", id}, {"serializedData", serializedData}, {"state", static_cast<int>(state)}}));
    return true;
}

void WebpageMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_webpages.begin(), m_webpages.end(),
                            [id](const std::unique_ptr<WebpageController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_webpages.end())
        return;

    (*it)->aboutToClose();
    m_webpages.erase(it, m_webpages.end());
}

void WebpageMediaController::registerNetworkReceiver()
{
    // ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

NetWorkReceiver::SendType WebpageMediaController::processMessage(NetworkMessageReader* msg)
{
    if(msg->category() == NetMsg::MediaCategory && msg->action() == NetMsg::AddMedia)
    {
        addWebpageController(MessageHelper::readWebPageData(msg));
    }
    return NetWorkReceiver::NONE;
}

int WebpageMediaController::managerCount() const
{
    return static_cast<int>(m_webpages.size());
}

std::vector<WebpageController*> WebpageMediaController::controllers() const
{
    std::vector<WebpageController*> vec;
    std::transform(m_webpages.begin(), m_webpages.end(), std::back_inserter(vec),
                   [](const std::unique_ptr<WebpageController>& ctrl) { return ctrl.get(); });
    return vec;
}

void WebpageMediaController::addWebpageController(const QHash<QString, QVariant>& params)
{
    QByteArray serializedData= params.value(QStringLiteral("serializedData")).toByteArray();

    auto id= params.value(QStringLiteral("id")).toString();

    std::unique_ptr<WebpageController> webCtrl(new WebpageController(id));

    if(params.contains(QStringLiteral("mode")))
    {
        auto mode= static_cast<WebpageController::SharingMode>(params.value(QStringLiteral("mode")).toInt());
        auto data= params.value(QStringLiteral("data")).toString();

        if(mode == WebpageController::Url)
            webCtrl->setPath(data);
        else if(mode == WebpageController::Html)
            webCtrl->setHtml(data);
    }
    if(params.contains(QStringLiteral("state")))
    {
        webCtrl->setState(static_cast<WebpageController::State>(params.value(QStringLiteral("state")).toInt()));
    }

    if(!serializedData.isEmpty())
    {
        IOHelper::readWebpageController(webCtrl.get(), serializedData);
    }

    auto webPage= webCtrl.get();
    connect(webCtrl.get(), &WebpageController::sharingModeChanged, this, [this, id]() { managePage(id); });
    connect(webCtrl.get(), &WebpageController::htmlChanged, this, [this, id]() { updatePage(id); });
    connect(webCtrl.get(), &WebpageController::htmlSharingChanged, this, [this, id]() { updatePage(id); });
    connect(webCtrl.get(), &WebpageController::pathChanged, this, [this, id]() { updatePage(id); });
    connect(webCtrl.get(), &WebpageController::urlSharingChanged, this, [this, id]() { updatePage(id); });
    connect(webCtrl.get(), &WebpageController::closeMe, this, [this, webPage]() {
        if(!m_undoStack)
            return;
        m_undoStack->push(new RemoveMediaControllerCommand(webPage, this));
    });

    connect(this, &WebpageMediaController::localIsGMChanged, webCtrl.get(), &WebpageController::setLocalGM);

    emit webpageControllerCreated(webCtrl.get());
    m_webpages.push_back(std::move(webCtrl));
}

void WebpageMediaController::managePage(const QString& id)
{
    auto it= std::find_if(m_webpages.begin(), m_webpages.end(),
                          [id](const std::unique_ptr<WebpageController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_webpages.end())
        return;

    if((*it)->sharingMode() == WebpageController::None)
        MessageHelper::closeMedia(id, type());
    else
        shareWebPage(id);
}

void WebpageMediaController::shareWebPage(const QString& id)
{
    auto it= std::find_if(m_webpages.begin(), m_webpages.end(),
                          [id](const std::unique_ptr<WebpageController>& ctrl) { return ctrl->uuid() == id; });

    if(it == m_webpages.end())
        return;

    MessageHelper::shareWebpage(it->get());
}

void WebpageMediaController::updatePage(const QString& id)
{
    auto it= std::find_if(m_webpages.begin(), m_webpages.end(),
                          [id](const std::unique_ptr<WebpageController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_webpages.end())
        return;

    MessageHelper::updateWebpage(it->get());
}
