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

#include "connectionretrydialog.h"
#include "ui_connectionretrydialog.h"

ConnectionRetryDialog::ConnectionRetryDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ConnectionRetryDialog)
{
    ui->setupUi(this);
    m_timer= new QTimer(this);
    m_msg= tr("Connection has failed! Connection Retry in %1s.");
    ui->m_label->setText(m_msg.arg(m_counter));

    connect(m_timer, &QTimer::timeout, this, &ConnectionRetryDialog::decreaseCounter);
    connect(this, &ConnectionRetryDialog::rejected, m_timer, &QTimer::stop);
}

ConnectionRetryDialog::~ConnectionRetryDialog()
{
    delete ui;
    delete m_timer;
}

quint16 ConnectionRetryDialog::timeOut() const
{
    return m_timer->interval();
}
quint16 ConnectionRetryDialog::counter() const
{
    return m_counter;
}

void ConnectionRetryDialog::setCounter(quint16 counter)
{
    if(m_counter == counter)
        return;
    m_counter= counter;
    emit counterChanged();
}

void ConnectionRetryDialog::startTimer()
{
    m_currentValue= m_counter;
    m_timer->start();
}
void ConnectionRetryDialog::setTimeOut(quint16 timeout)
{
    if(timeout == m_timer->interval())
        return;
    m_timer->setInterval(timeout);
    emit timeOutChanged();
}
void ConnectionRetryDialog::decreaseCounter()
{
    --m_currentValue;
    ui->m_label->setText(m_msg.arg(m_counter));
    if(0 >= m_currentValue)
    {
        emit tryConnection();
        accept();
        m_currentValue= m_counter;
    }
}
