/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

#ifndef MAP_FRAME_H
#define MAP_FRAME_H

#include <QAction>
#include <QImage>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QPointer>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "docks/vmaptoolbar.h"
#include "mediacontainers/mediacontainer.h"
#include "rwidgets/customs/rgraphicsview.h"
#include "rwidgets/customs/vmap.h"
#include "rwidgets/rwidgets_global.h"
#include "toolbars/vtoolbar.h"

class VectorialMapController;

/**
 * @brief displays, stores and manages a map and its items
 *
 */
class RWIDGET_EXPORT VMapFrame : public MediaContainer
{
    Q_OBJECT
    Q_PROPERTY(VMap* map READ map CONSTANT)
    Q_PROPERTY(ToolBox* toolBox READ toolBox CONSTANT)
    Q_PROPERTY(VmapTopBar* topBar READ topBar CONSTANT)
public:
    VMapFrame(VectorialMapController* ctrl, QWidget* parent= nullptr);
    virtual ~VMapFrame() override;
    bool openFile(const QString& file);

    ToolBox* toolBox() const;
    VmapTopBar* topBar() const;
    VMap* map() const;

    QPointF mapFromScene(const QPointF& point);

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

private: // functions
    void setupUi();

private:
    QPointer<VectorialMapController> m_ctrl;
    std::unique_ptr<VMap> m_vmap;
    std::unique_ptr<RGraphicsView> m_graphicView;
    std::unique_ptr<ToolBox> m_toolbox;
    std::unique_ptr<VmapTopBar> m_topBar;
    QTimer m_timer;
};
#endif
