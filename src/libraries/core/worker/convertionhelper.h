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
#ifndef CONVERTIONHELPER_H
#define CONVERTIONHELPER_H

#include <QColor>
#include <QDataStream>
#include <QFont>
#include <QImage>
#include <QPointF>
#include <QRectF>
#include <QSize>

#include "controller/view_controller/sharednotecontroller.h"
#include "data/charactervision.h"
#include "data/link.h"
#include "media/mediatype.h"
#include "network/networkmessagewriter.h"

namespace Helper
{
template <typename T>
void variantToType(const T& val, NetworkMessageWriter& msg)
{
    msg.string32(val);
}

template <>
void variantToType<bool>(const bool& val, NetworkMessageWriter& msg);
template <>
void variantToType<qreal>(const qreal& val, NetworkMessageWriter& msg);
template <>
void variantToType<int>(const int& val, NetworkMessageWriter& msg);
template <>
void variantToType<Core::ScaleUnit>(const Core::ScaleUnit& val, NetworkMessageWriter& msg);
template <>
void variantToType<Core::PermissionMode>(const Core::PermissionMode& val, NetworkMessageWriter& msg);
template <>
void variantToType<Core::GridPattern>(const Core::GridPattern& val, NetworkMessageWriter& msg);
template <>
void variantToType<Core::Layer>(const Core::Layer& val, NetworkMessageWriter& msg);
template <>
void variantToType<Core::VisibilityMode>(const Core::VisibilityMode& val, NetworkMessageWriter& msg);
template <>
void variantToType<QColor>(const QColor& val, NetworkMessageWriter& msg);
template <>
void variantToType<QImage>(const QImage& val, NetworkMessageWriter& msg);
template <>
void variantToType<QPointF>(const QPointF& val, NetworkMessageWriter& msg);
template <>
void variantToType<QRectF>(const QRectF& val, NetworkMessageWriter& msg);
template <>
void variantToType<quint16>(const quint16& val, NetworkMessageWriter& msg);
template <>
void variantToType<QByteArray>(const QByteArray& val, NetworkMessageWriter& msg);
template <>
void variantToType<QFont>(const QFont& val, NetworkMessageWriter& msg);
template <>
void variantToType<std::vector<QPointF>>(const std::vector<QPointF>& val, NetworkMessageWriter& msg);
template <>
void variantToType<CharacterVision::SHAPE>(const CharacterVision::SHAPE& shape, NetworkMessageWriter& msg);
template <>
void variantToType<QSize>(const QSize& size, NetworkMessageWriter& msg);
template <>
void variantToType<ParticipantModel::Permission>(const ParticipantModel::Permission& perm, NetworkMessageWriter& msg);
template <>
void variantToType<mindmap::Link::Direction>(const mindmap::Link::Direction& perm, NetworkMessageWriter& msg);

} // namespace Helper

#endif // CONVERTIONHELPER_H
