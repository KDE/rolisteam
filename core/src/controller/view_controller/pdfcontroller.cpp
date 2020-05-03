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
#include "pdfcontroller.h"

#include "data/cleveruri.h"
#include "worker/iohelper.h"

PdfController::PdfController(const QString& id, const QString& path, const QByteArray& data, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::PDF, parent)
{
    if(!path.isEmpty())
    {
        m_data= IOHelper::loadFile(path);
    }
    else if(!data.isEmpty())
    {
        m_data= data;
    }
    emit dataChanged(m_data);
}

PdfController::~PdfController()= default;

void PdfController::saveData() const {}

void PdfController::loadData() const {}

QByteArray PdfController::data() const
{
    return m_data;
}

void PdfController::shareImageIntoImage(const QPixmap& image) {}

void PdfController::shareImageIntoMap(const QPixmap& image) {}

void PdfController::shareImageIntoVMap(const QPixmap& image) {}

void PdfController::shareAsPdf() {}
