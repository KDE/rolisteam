/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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

#ifndef CONNECTIONRETRYDIALOG_H
#define CONNECTIONRETRYDIALOG_H

#include "rwidgets_global.h"
#include <QDialog>
#include <QTimer>
namespace Ui
{
class ConnectionRetryDialog;
}
/**
 * @brief The ConnectionRetryDialog class
 */
class RWIDGET_EXPORT ConnectionRetryDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(quint16 counter READ counter WRITE setCounter NOTIFY counterChanged)
    Q_PROPERTY(quint16 timeout READ timeOut WRITE setTimeOut NOTIFY timeOutChanged)
public:
    explicit ConnectionRetryDialog(QWidget* parent= 0);
    ~ConnectionRetryDialog();

    quint16 timeOut() const;
    quint16 counter() const;

public slots:
    void setCounter(quint16 counter= 0);
    void startTimer();
    void setTimeOut(quint16 timeout);

private slots:
    void decreaseCounter();

signals:
    void tryConnection();
    void timeOutChanged();
    void counterChanged();

private:
    Ui::ConnectionRetryDialog* ui;
    quint16 m_counter{11};
    quint16 m_currentValue{0};
    QTimer* m_timer;
    QString m_msg;
};

#endif // CONNECTIONRETRYDIALOG_H
