/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include <QDebug>
#include <QMenu>
#include <QVBoxLayout>

#include "playerspanel.h"
#include "userlistview.h"

#include "controller/playercontroller.h"
#include "data/character.h"
#include "data/person.h"
#include "data/player.h"
#include "ui_playerspanel.h"
#include "userlist/playermodel.h"
#include "widgets/delegate.h"
#include "widgets/persondialog.h"

/********************
 * PlayersPanel *
 ********************/

PlayersPanel::PlayersPanel(PlayerController* ctrl, QWidget* parent)
    : QDockWidget(parent), m_ui(new Ui::PlayersPanel), m_ctrl(ctrl)
{
    m_ui->setupUi(this);
    m_ui->m_playerView->setPlayerController(ctrl);

    m_ui->m_addBtn->setDefaultAction(m_ui->m_addLocalCharacter);
    m_ui->m_addLocalCharacter->setIcon(QIcon::fromTheme("add_round"));

    m_ui->m_removeBtn->setDefaultAction(m_ui->m_removeLocalCharacter);
    m_ui->m_removeLocalCharacter->setIcon(QIcon::fromTheme("delete"));

    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setWindowTitle(tr("Player List"));
    setObjectName("PlayersPanel");

    setConnection();
}

PlayersPanel::~PlayersPanel() {}

void PlayersPanel::setConnection()
{
    connect(m_ui->m_playerView, &UserListView::runDiceForCharacter, this, &PlayersPanel::runDiceForCharacter);

    m_ui->m_playerView->setModel(m_ctrl->model());
    m_selectionModel= m_ui->m_playerView->selectionModel();
    m_ui->m_playerView->setHeaderHidden(true);

    // Actions
    connect(m_selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this,
            SLOT(selectAnotherPerson(QModelIndex)));
    connect(m_ui->m_addLocalCharacter, &QAction::triggered, m_ctrl, &PlayerController::addLocalCharacter);
    connect(m_ui->m_removeLocalCharacter, &QAction::triggered, this,
            [this]() { m_ctrl->removeLocalCharacter(m_ui->m_playerView->currentIndex()); });

    selectAnotherPerson(m_ui->m_playerView->currentIndex());
}

void PlayersPanel::selectAnotherPerson(const QModelIndex& current)
{
    m_ui->m_removeLocalCharacter->setEnabled(current.isValid() && current.parent().isValid()
                                             && current.data(PlayerModel::LocalRole).toBool());
}
