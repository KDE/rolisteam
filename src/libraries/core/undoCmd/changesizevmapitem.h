/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#ifndef CHANGESIZEVMAPITEMCOMMAND_H
#define CHANGESIZEVMAPITEMCOMMAND_H

#include "controller/view_controller/vectorialmapcontroller.h"
#include <QPointF>
#include <QPointer>
#include <QUndoCommand>

struct ChangeSizeData
{
    vmap::VisualItemController* m_ctrl;
    QPointF m_move;
    QPointF m_resetMove;
};

class ChangeSizeVmapItemCommand : public QUndoCommand
{
public:
    ChangeSizeVmapItemCommand(const QList<vmap::VisualItemController*>& list, VectorialMapController::Method method,
                              const QPointF& mousePos, QUndoCommand* parent= nullptr);

    void undo() override;
    void redo() override;

private:
    QPointer<VectorialMapController> m_vmapCtrl;
    std::vector<ChangeSizeData> m_data;
};

#endif // CHANGESIZEVMAPITEMCOMMAND_H
