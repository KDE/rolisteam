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
#ifndef ADDNODECOMMAND_H
#define ADDNODECOMMAND_H

#include <QPointer>
#include <QUndoCommand>

class MindMapUpdater;
namespace mindmap
{
class MindNode;
class Link;
class BoxModel;
class LinkModel;
class AddNodeCommand : public QUndoCommand
{
public:
    AddNodeCommand(const QString& idmap, MindMapUpdater* updater, BoxModel* nodeModel, LinkModel* linkModel,
                   const QString& idParent);

    void setData(const QString& text, const QString& imgUrl);

    void undo() override;
    void redo() override;

private:
    QString m_idmap;
    QPointer<MindMapUpdater> m_updater;
    QPointer<mindmap::MindNode> m_mindNode;
    QPointer<Link> m_link;
    BoxModel* m_nodeModel= nullptr;
    LinkModel* m_linkModel= nullptr;

    QString m_text;
    QString m_imgUrl;

    QString m_idParent;
};

} // namespace mindmap
#endif // ADDNODECOMMAND_H
