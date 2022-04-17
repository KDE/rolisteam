/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
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
#ifndef CHARACTERSTATE_H
#define CHARACTERSTATE_H

#include <QColor>
#include <QPixmap>
#include <QString>

#include "network_global.h"
/**
 * @brief The CharacterState class stores all data for CharacterState
 */
class NETWORK_EXPORT CharacterState
{
public:
    /**
     * @brief CharacterState
     */
    CharacterState();
    /**
     * @brief CharacterState
     */
    CharacterState(const CharacterState& copy);
    /**
     * @brief setLabel
     * @param str
     */
    void setLabel(QString str);
    /**
     * @brief setColor
     * @param str
     */
    void setColor(QColor str);
    /**
     * @brief setImage
     * @param str
     */
    void setImagePath(const QString& str);
    void setPixmap(const QPixmap& pix);
    /**
     * @brief getLabel
     * @return
     */
    const QString& label() const;
    /**
     * @brief getColor
     * @return
     */
    const QColor& color() const;
    /**
     * @brief getImage
     * @return
     */
    const QString& imagePath() const;

    const QPixmap& pixmap() const;

    bool hasImage() const;

    QString id() const;
    void setId(const QString& id);

private:
    QString m_id;
    QString m_label;
    QColor m_color;
    QString m_imagePath;
    QPixmap m_pixmap;
};

Q_DECLARE_METATYPE(CharacterState)
Q_DECLARE_METATYPE(CharacterState*)

#endif // CHARACTERSTATE_H
