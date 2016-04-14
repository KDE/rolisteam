/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QMainWindow>
#include "canvas.h"
#include "fieldmodel.h"
#include "rolisteamimageprovider.h"
#include "field.h"
#include "charactersheetmodel.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum EDITION_TOOL {ADDFIELD,SELECT};
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    Field* addFieldAt(QPoint pos);
    bool qmlGeneration() const;
    void setQmlGeneration(bool qmlGeneration);

    void updatePageSelector();
public slots:
    void setCurrentTool();

    void save();
    void saveAs();

    void open();
    void generateQML(QString& qml);

    void showQML();
    void showQMLFromCode();

    void saveQML();
    void openQML();
    void setImage();
    void rollDice(QString cmd);
    void addPage();
    void removePage();
    void currentPageChanged(int);
    void menuRequested(const QPoint &pos);
protected:

    bool eventFilter(QObject *, QEvent *);

protected slots:
    void columnAdded();
private:
    Ui::MainWindow *ui;
    QList<Canvas*> m_canvasList;
    QGraphicsView* m_view;
    EDITION_TOOL m_currentTool;
    QPoint m_startField;
    QList<QPixmap> m_pixList;
    FieldModel* m_model;
    QString m_filename;
    bool m_qmlGeneration;
    RolisteamImageProvider* m_imgProvider;
    CharacterSheetModel* m_characterModel;
    int m_currentPage;

    //Action
    QAction* m_addCharacter;
};

#endif // MAINWINDOW_H

