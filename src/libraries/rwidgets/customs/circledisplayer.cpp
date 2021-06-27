/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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
#include <QDebug>
#include <QtGui>

#include "widgets/circledisplayer.h"

#define MARGIN_NPC 10
#define MARGIN_PC 5
#define PEN_SIZE 4

CircleDisplayer::CircleDisplayer(QWidget* parent, bool filled, int minimum, int maximum) : QWidget(parent)
{
    m_currentDiameter= minimum;
    m_minimumDiameter= minimum;
    m_maximumDiameter= maximum;

    m_full= filled;
}

void CircleDisplayer::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    int displayedDiameter;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if(m_full)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        displayedDiameter= m_currentDiameter;
    }
    else
    {
        QPen pen(Qt::black);
        pen.setWidth(4);
        painter.setPen(pen);
        painter.setBrush(Qt::white);
        displayedDiameter= m_currentDiameter - m_minimumDiameter + 1;
    }
    painter.drawEllipse(
        (width() - m_currentDiameter) / 2, (height() - m_currentDiameter) / 2, m_currentDiameter, m_currentDiameter);
    painter.setPen(Qt::darkGray);
    painter.drawText(0, 0, width(), height(), Qt::AlignRight | Qt::AlignBottom, QString::number(displayedDiameter));
}
void CircleDisplayer::wheelEvent(QWheelEvent* event)
{
    int step= event->delta() / 8;
    if(step + m_currentDiameter > m_maximumDiameter)
        m_currentDiameter= m_maximumDiameter;
    else if(step + m_currentDiameter < m_minimumDiameter)
        m_currentDiameter= 0;
    else
        m_currentDiameter+= step;
    emit diameterChanged(m_currentDiameter);
    update();
}
void CircleDisplayer::changeDiameter(int diameter)
{
    m_currentDiameter= diameter;
    update();
}
