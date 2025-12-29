/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#ifndef REMOVENODECOMMAND_H
#define REMOVENODECOMMAND_H

#include <QCoreApplication>
#include <QJsonArray>
#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <mindmap/mindmap_global.h>

namespace mindmap
{
class MindItem;
class LinkController;
class MindItemModel;
class MINDMAP_EXPORT RemoveNodeCommand : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(RemoveNodeCommand)
public:
    RemoveNodeCommand(const std::vector<QPointer<MindItem>>& selection, MindItemModel* nodeModel);
    void undo() override;
    void redo() override;

private:
    QPointer<MindItemModel> m_nodeModel;
    QJsonArray m_nodeData;
    QJsonArray m_linkData;
};
} // namespace mindmap
#endif // REMOVENODECOMMAND_H
