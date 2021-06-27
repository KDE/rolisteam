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
#ifndef AUTOSAVECONTROLLER_H
#define AUTOSAVECONTROLLER_H

#include <QObject>
#include <QPointer>
#include <memory>

class PreferencesManager;
class QTimer;
class AutoSaveController : public QObject
{
    Q_OBJECT
public:
    explicit AutoSaveController(PreferencesManager* pref, QObject* parent= nullptr);

signals:
    void saveData();

private:
    PreferencesManager* m_preferences;
    std::unique_ptr<QTimer> m_timer;
};

#endif // AUTOSAVECONTROLLER_H
