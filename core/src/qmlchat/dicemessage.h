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
#ifndef DICEMESSAGE_H
#define DICEMESSAGE_H

#include <QJsonObject>
#include <QObject>

#include "messageinterface.h"

namespace InstantMessaging
{

class DiceMessage : public MessageBase
{
    Q_OBJECT
    // Q_PROPERTY(qreal result READ result WRITE setResult NOTIFY resultChanged)
public:
    DiceMessage(const QString& ownerId, const QString& writer, const QDateTime& time, QObject* parent= nullptr);

    QString text() const override;

public slots:
    void setText(const QString& text) override;

private:
    QJsonObject m_data;
};

} // namespace InstantMessaging
#endif // DICEMESSAGE_H