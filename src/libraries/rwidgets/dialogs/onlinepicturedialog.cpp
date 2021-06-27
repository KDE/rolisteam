/***************************************************************************
 * Copyright (C) 2015 by Renaud Guezennec                                   *
 * https://rolisteam.org/contact                      *
 *                                                                          *
 *  This file is part of rcm                                                *
 *                                                                          *
 * Rolisteam is free software; you can redistribute it and/or modify              *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include <QDebug>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include "onlinepicturedialog.h"
#include "ui_onlinepicturedialog.h"

OnlinePictureDialog::OnlinePictureDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::OnlinePictureDialog), m_isPosting(false)
{
    ui->setupUi(this);
    connect(ui->m_urlField, SIGNAL(editingFinished()), this, SLOT(uriChanged()));
    connect(ui->m_downloadPush, SIGNAL(clicked()), this, SLOT(uriChanged()));
#ifdef HAVE_QT_NETWORK
    m_manager.reset(new QNetworkAccessManager());
    connect(m_manager.get(), &QNetworkAccessManager::finished, this, &OnlinePictureDialog::replyFinished);
#endif
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->scrollArea->setAlignment(Qt::AlignCenter);
    m_imageViewerLabel= new QLabel(this);
    m_imageViewerLabel->setPixmap(QPixmap(QString::fromUtf8(":/resources/images/preview.png")));
    m_imageViewerLabel->setLineWidth(0);
    m_imageViewerLabel->setFrameStyle(QFrame::NoFrame);
    m_imageViewerLabel->setScaledContents(true);
    m_imageViewerLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageViewerLabel->resize(m_pix.size());

    ui->scrollArea->setWidget(m_imageViewerLabel);
}

OnlinePictureDialog::~OnlinePictureDialog()
{
    delete ui;
}
void OnlinePictureDialog::uriChanged()
{
    QFileInfo info(ui->m_urlField->text());
    // if(ui->m_titleLineEdit->text().isEmpty())
    {
        ui->m_titleLineEdit->setText(info.baseName());
    }
    if((!m_isPosting) || (ui->m_urlField->text() != m_postingStr))
    {
        m_isPosting= true;
        m_postingStr= ui->m_urlField->text();
        if(m_manager)
            m_manager->get(QNetworkRequest(QUrl(ui->m_urlField->text())));
    }
}
void OnlinePictureDialog::replyFinished(QNetworkReply* reply)
{
    m_isPosting= false;
    QByteArray data= reply->readAll();
    QPixmap map;
    bool ok= map.loadFromData(data);
    if(ok)
    {
        m_data= data;
        m_imageViewerLabel->setPixmap(map);
        m_imageViewerLabel->resize(map.size());
        m_pix= map;

        resizeLabel();
        update();
    }
}
QString OnlinePictureDialog::getPath()
{
    return ui->m_urlField->text();
}

QByteArray OnlinePictureDialog::getData()
{
    return m_data;
}
QString OnlinePictureDialog::getTitle()
{
    return ui->m_titleLineEdit->text();
}
void OnlinePictureDialog::resizeLabel()
{
    int w= ui->scrollArea->viewport()->rect().width();
    int h= ui->scrollArea->viewport()->rect().height();

    double ratioImage= (double)m_pix.size().width() / m_pix.size().height();
    double ratioImageBis= (double)m_pix.size().height() / m_pix.size().width();

    if(w > h * ratioImage)
    {
        m_imageViewerLabel->resize(h * ratioImage, h);
    }
    else
    {
        m_imageViewerLabel->resize(w, w * ratioImageBis);
    }
}
void OnlinePictureDialog::resizeEvent(QResizeEvent* event)
{
    resizeLabel();
    QDialog::resizeEvent(event);
}
