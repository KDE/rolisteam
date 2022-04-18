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
#include "controller/view_controller/pdfcontroller.h"

#include "utils/iohelper.h"

#include <QBuffer>

PdfController::PdfController(const QString& id, const QUrl& path, const QByteArray& data, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::PDF, parent)
{
    if(!path.isEmpty())
    {
        setData(utils::IOHelper::loadFile(path.toLocalFile()));
    }
    else if(!data.isEmpty())
    {
        setData(data);
    }
}

PdfController::~PdfController()= default;

QByteArray PdfController::data() const
{
    return m_data;
}

QBuffer* PdfController::buffer()
{
    return new QBuffer(&m_data);
}

qreal PdfController::zoomFactor() const
{
    return m_zoom;
}

void PdfController::zoomIn()
{
    setZoomFactor(m_zoom + 0.02);
}

void PdfController::zoomOut()
{
    setZoomFactor(m_zoom - 0.02);
}

void PdfController::setData(const QByteArray& data)
{
    if(data == m_data)
        return;
    m_data= data;
    emit dataChanged(m_data);
}

void PdfController::setZoomFactor(qreal zoom)
{
    if(qFuzzyCompare(m_zoom, zoom))
        return;

    m_zoom= zoom;
    emit zoomFactorChanged(m_zoom);
}

void PdfController::shareImageIntoImage(const QPixmap& image) {}

void PdfController::shareImageIntoMap(const QPixmap& image) {}

void PdfController::shareImageIntoVMap(const QPixmap& image) {}

void PdfController::shareAsPdf() {}
