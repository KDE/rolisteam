#include "channellistpanel.h"
#include "ui_channellistpanel.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>

#include "common/logcategory.h"
#include "network/channel.h"
#include "network/serverconnection.h"
#include "preferences/preferencesmanager.h"

ChannelListPanel::ChannelListPanel(PreferencesManager* preferences, NetworkController* ctrl, QWidget* parent)
    : QWidget(parent), ui(new Ui::ChannelListPanel), m_ctrl(ctrl), m_pref(preferences)
{
    ui->setupUi(this);
    ui->m_channelView->setModel(m_ctrl->channelModel());
    ui->m_channelView->setAlternatingRowColors(true);
    ui->m_channelView->setHeaderHidden(true);
    ui->m_channelView->setAcceptDrops(true);
    ui->m_channelView->setDragEnabled(true);
    ui->m_channelView->setDropIndicatorShown(true);
    ui->m_channelView->setDefaultDropAction(Qt::MoveAction);
    ui->m_channelView->setDragDropMode(QAbstractItemView::InternalMove);

    ui->m_channelView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->m_channelView, &QTreeView::customContextMenuRequested, this, &ChannelListPanel::showCustomMenu);
    connect(m_ctrl->channelModel(), &ChannelModel::localPlayerGMChanged, this,
            &ChannelListPanel::CurrentChannelGmIdChanged);

    m_edit= new QAction(tr("Edit Channel"), this);
    m_lock= new QAction(tr("Lock Channel"), this);
    m_join= new QAction(tr("Switch to channel"), this);
    m_channelPassword= new QAction(tr("Set Channel Password"), this);
    m_addChannel= new QAction(tr("Add channel"), this);
    m_addSubchannel= new QAction(tr("Add subchannel"), this);
    m_deleteChannel= new QAction(tr("Delete Channel"), this);
    m_setDefault= new QAction(tr("Set Default"), this);
    m_admin= new QAction(tr("Log as admin"), this);
    m_kick= new QAction(tr("Kick User"), this);
    m_ban= new QAction(tr("Ban User"), this);
    m_resetChannel= new QAction(tr("Reset Data Channel"), this);
    m_moveUserToCurrentChannel= new QAction(tr("Move User"), this);

    connect(m_kick, &QAction::triggered, this, &ChannelListPanel::kickUser);
    connect(m_setDefault, &QAction::triggered, m_ctrl,
            [this]()
            {
                Channel* parent= getChannel(m_index);
                if(!parent)
                    return;
                m_ctrl->defineChannelAsDefault(parent->uuid());
            });
    connect(m_ban, &QAction::triggered, this, &ChannelListPanel::banUser);
    connect(m_edit, &QAction::triggered, this, &ChannelListPanel::editChannel);
    connect(m_addChannel, &QAction::triggered, m_ctrl, [this]() { emit m_ctrl->addChannel(QString()); });
    connect(m_addSubchannel, &QAction::triggered, m_ctrl,
            [this]()
            {
                Channel* parent= getChannel(m_index);
                if(!parent)
                    return;

                emit m_ctrl->addChannel(parent->uuid());
            });
    connect(m_deleteChannel, &QAction::triggered, this, &ChannelListPanel::deleteChannel);
    connect(m_lock, &QAction::triggered, this, &ChannelListPanel::lockChannel);
    connect(m_join, &QAction::triggered, this, &ChannelListPanel::joinChannel);
    connect(m_admin, &QAction::triggered, this, &ChannelListPanel::logAsAdmin);
    connect(m_channelPassword, &QAction::triggered, this, &ChannelListPanel::setPasswordOnChannel);
    connect(m_resetChannel, &QAction::triggered, this, &ChannelListPanel::resetChannel);
    connect(m_moveUserToCurrentChannel, &QAction::triggered, this, &ChannelListPanel::moveUserToCurrent);
}

ChannelListPanel::~ChannelListPanel()
{
    delete ui;
}

void ChannelListPanel::showCustomMenu(const QPoint& pos)
{
    if(!m_ctrl)
        return;

    QMenu menu(this);

    auto localId= m_ctrl->localId();

    m_index= ui->m_channelView->indexAt(pos);

    TreeItem* dataItem= indexToPointer<TreeItem*>(m_index);
    auto channel= dynamic_cast<Channel*>(dataItem);

    auto isOnChannel= static_cast<bool>(channel);
    auto isCurrentChannel= isOnChannel ? static_cast<bool>((channel->getClientById(localId))) : false;
    auto isOnUser= dataItem && !isOnChannel ? static_cast<bool>(dataItem) : false;

    auto isUserOnCurrentChannel= false;
    {
        auto model= m_ctrl->channelModel();
        auto local= model->getItemById(localId);
        if(local && dataItem)
        {
            auto localPlayerChannel= dynamic_cast<Channel*>(local->getParentItem());
            auto selectedPlayerChannel= dynamic_cast<Channel*>(dataItem->getParentItem());

            if(localPlayerChannel && selectedPlayerChannel)
                isUserOnCurrentChannel= (localPlayerChannel->uuid() == selectedPlayerChannel->uuid());
        }
    }

    auto isAdmin= m_ctrl->isAdmin();
    auto channelIsLock= isOnChannel ? channel->locked() : false;
    auto channelIsDefault= isOnChannel ? m_ctrl->defaultChannelId() == channel->uuid() : false;
    auto isLocal= dataItem ? (localId == dataItem->uuid()) : false;
    auto channelIsEmpty= isOnChannel ? channel->childCount() == 0 : true;

    menu.addAction(m_join);
    menu.addSeparator();
    menu.addAction(m_lock);
    menu.addAction(m_edit);
    menu.addAction(m_resetChannel);
    menu.addAction(m_setDefault);
    menu.addAction(m_channelPassword);
    menu.addSeparator();
    menu.addAction(m_addChannel);
    menu.addAction(m_deleteChannel);
    menu.addSeparator();
    menu.addAction(m_kick);
    menu.addAction(m_ban);
    menu.addAction(m_moveUserToCurrentChannel);

    if(!isAdmin)
    {
        menu.addSeparator();
        menu.addAction(m_admin);
    }

    m_lock->setText(channelIsLock ? tr("Unlock channel") : tr("Lock Channel"));

    m_join->setEnabled(!isCurrentChannel && isOnChannel);
    m_lock->setEnabled(isAdmin && isOnChannel);
    m_edit->setEnabled(isAdmin && isOnChannel);
    m_resetChannel->setEnabled(isAdmin && isOnChannel);
    m_deleteChannel->setEnabled(isAdmin && isOnChannel && channelIsEmpty);
    m_setDefault->setEnabled(isAdmin && isOnChannel && !channelIsDefault);
    m_channelPassword->setEnabled(isAdmin && isOnChannel);
    m_kick->setEnabled(isAdmin && isOnUser);
    m_ban->setEnabled(isAdmin && isOnUser);
    qDebug() << "admin" << isAdmin << "onuser" << isOnUser << "isuser on current channel" << isUserOnCurrentChannel
             << "isLocal" << isLocal;
    m_moveUserToCurrentChannel->setEnabled(isAdmin && isOnUser && !isUserOnCurrentChannel && !isLocal);
    m_addChannel->setEnabled(isAdmin && !isOnChannel && !isOnUser);

    menu.exec(ui->m_channelView->mapToGlobal(pos));
}

void ChannelListPanel::kickUser()
{
    if(!m_ctrl->isAdmin())
        return;

    if(!m_index.isValid())
        return;

    ServerConnection* item= getClient(m_index);
    if(item == nullptr)
        return;

    QString id= item->uuid();
    QString idPlayer= item->playerId();
    m_ctrl->kickUser(id, idPlayer);
}
void ChannelListPanel::lockChannel()
{
    if(!m_ctrl->isGM() || !m_index.isValid())
        return;

    Channel* item= getChannel(m_index);
    if(item == nullptr)
        return;
    QString id= item->uuid();
    if(id.isEmpty())
        return;

    auto action= item->locked() ? NetMsg::UnlockChannel : NetMsg::LockChannel;
    m_ctrl->lockChannel(id, action);
}

template <typename T>
T ChannelListPanel::indexToPointer(QModelIndex index)
{
    T item= static_cast<T>(index.internalPointer());
    return item;
}

ServerConnection* ChannelListPanel::getClient(QModelIndex index)
{
    auto item= indexToPointer<TreeItem*>(index);
    if(item->isLeaf())
    {
        return static_cast<ServerConnection*>(index.internalPointer());
    }
    return nullptr;
}

Channel* ChannelListPanel::getChannel(QModelIndex index)
{
    auto item= indexToPointer<TreeItem*>(index);
    if(!item->isLeaf())
    {
        return static_cast<Channel*>(index.internalPointer());
    }
    return nullptr;
}

void ChannelListPanel::banUser()
{
    if(!m_ctrl->isAdmin() || !m_index.isValid())
        return;

    ServerConnection* item= getClient(m_index); /// static_cast<ServerConnection*>(m_index.internalPointer());
    if(!item)
        return;

    QString id= item->uuid();
    QString idPlayer= item->playerId();

    if(id.isEmpty())
        return;

    m_ctrl->banUser(id, idPlayer);
}

void ChannelListPanel::logAsAdmin()
{
    QString pwadmin;

    if(m_pref)
        pwadmin= m_pref->value(QString("adminPassword_for_%1").arg(m_serverName), QString()).toString();

    if(pwadmin.isEmpty())
        pwadmin= QInputDialog::getText(this, tr("Admin Password"), tr("Password"), QLineEdit::Password);

    m_ctrl->sendOffLoginAdmin(pwadmin);
}

void ChannelListPanel::editChannel()
{
    auto chan= getChannel(m_index);
    if(!chan)
        return;

    if(m_ctrl->isAdmin())
    {
        ui->m_channelView->edit(m_index);
    }
}

QString ChannelListPanel::serverName() const
{
    return m_serverName;
}

void ChannelListPanel::setServerName(const QString& serverName)
{
    m_serverName= serverName;
}

void ChannelListPanel::resetChannel()
{
    if(!m_index.isValid())
        return;

    if(m_ctrl->groups() == NetworkController::VIEWER)
        return;

    Channel* item= getChannel(m_index);
    if(item == nullptr)
        return;

    QString id= item->uuid();

    m_ctrl->resetChannel(id);
}

void ChannelListPanel::moveUserToCurrent()
{
    if(!m_index.isValid())
        return;

    TreeItem* dataItem= indexToPointer<TreeItem*>(m_index);
    if(!dataItem)
        return;
    if(!dataItem->isLeaf())
        return;

    auto playerToMove= dataItem->uuid();
    auto model= m_ctrl->channelModel();
    if(!model)
        return;

    auto local= model->getItemById(m_ctrl->localId());

    if(!local)
        return;

    auto destChannel= dynamic_cast<Channel*>(local->getParentItem());

    if(!destChannel)
        return;

    auto destId= destChannel->uuid();

    prepareJoin(playerToMove, destId, !destChannel->password().isEmpty());
}

void ChannelListPanel::deleteChannel()
{
    if(!m_index.isValid())
        return;

    Channel* item= getChannel(m_index);
    if(nullptr == item)
        return;
    if(item->childCount() > 0)
    {
        qCWarning(NetworkCat) << tr("Only empty channel can be removed");
        return;
    }
    QString id= item->uuid();

    m_ctrl->deleteChannel(id);
}

void ChannelListPanel::setPasswordOnChannel()
{
    if(!m_ctrl->isAdmin() && !m_index.isValid())
        return;

    Channel* item= getChannel(m_index);

    if(nullptr == item)
        return;

    auto pw= QInputDialog::getText(this, tr("Channel Password"),
                                   tr("Password for channel: %1 - leave empty for no password").arg(item->name()),
                                   QLineEdit::Password, item->password());

    auto pwA= pw.isEmpty() ? QByteArray() : QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha3_512);
    m_ctrl->definePasswordOnChannel(item->uuid(), pwA);
}

void ChannelListPanel::joinChannel()
{
    if(!m_index.isValid())
        return;

    Channel* item= getChannel(m_index);
    if(nullptr == item)
        return;

    prepareJoin(m_ctrl->localId(), item->uuid(), !item->password().isEmpty());
}

void ChannelListPanel::prepareJoin(const QString& user, const QString& destChannel, bool needPassword)
{
    QByteArray pw;
    if(needPassword)
        pw= QInputDialog::getText(this, tr("Channel Password"), tr("Destination Channel required password"),
                                  QLineEdit::Password)
                .toUtf8();

    auto pwA= pw.isEmpty() ? QByteArray() : QCryptographicHash::hash(pw, QCryptographicHash::Sha3_512);
    m_ctrl->joinChannel(user, destChannel, pwA);
}
