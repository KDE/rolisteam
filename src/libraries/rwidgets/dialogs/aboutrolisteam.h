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

#ifndef ABOUTROLISTEAM_H
#define ABOUTROLISTEAM_H

#include <QDialog>

namespace Ui
{
    class AboutRolisteam;
}
/**
 * @brief The AboutRolisteam class shows the dialog box about rolisteam
 * Its goal and the list of author and translators.
 */
class AboutRolisteam : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief AboutRolisteam
     * @param version
     * @param parent
     */
    explicit AboutRolisteam(QString version, QWidget* parent= 0);
    /**
     * @brief ~AboutRolisteam
     */
    virtual ~AboutRolisteam();

private:
    Ui::AboutRolisteam* ui;
};

#endif // ABOUTROLISTEAM_H
