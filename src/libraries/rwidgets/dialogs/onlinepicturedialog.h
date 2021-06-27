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
#ifndef ONLINEPICTUREDIALOG_H
#define ONLINEPICTUREDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QNetworkAccessManager>
#include <memory>

namespace Ui
{
class OnlinePictureDialog;
}
/**
 * @brief The OnlinePictureDialog class is dedicated to download image from the Internet. The user copies/pastes the
 * url, then the download starts. A preview of the image shows the image to the user. If the user valids the picture is
 * added in rolisteam. otherwise, the image is dropped.
 */
class OnlinePictureDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief OnlinePictureDialog
     * @param parent
     */
    explicit OnlinePictureDialog(QWidget* parent= nullptr);
    /**
     * @brief ~OnlinePictureDialog
     */
    virtual ~OnlinePictureDialog();
    /**
     * @brief getPath
     * @return
     */
    QString getPath();
    /**
     * @brief getPixmap
     * @return
     */
    QByteArray getData();
    /**
     * @brief getTitle
     * @return
     */
    QString getTitle();
private slots:
    /**
     * @brief uriChanged
     */
    void uriChanged();
    /**
     * @brief replyFinished
     * @param reply
     */
    void replyFinished(QNetworkReply* reply);

protected:
    /**
     * @brief resizeLabel
     */
    void resizeLabel();
    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event);

private:
    Ui::OnlinePictureDialog* ui;
    std::unique_ptr<QNetworkAccessManager> m_manager;
    QPixmap m_pix;
    double m_zoomLevel;
    QLabel* m_imageViewerLabel;
    bool m_isPosting;
    QString m_postingStr;
    QByteArray m_data;
};

#endif // ONLINEPICTUREDIALOG_H
