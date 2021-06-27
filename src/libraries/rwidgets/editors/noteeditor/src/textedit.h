/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QtPrintSupport/QPrinter>

#include "noteeditor/format_OO_oasis_/OOReader.h"

#include <QTextDocumentWriter>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QMenu)
class NoteController;

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(NoteController* note, QWidget* parent= 0);
    QString getFilter() const;

    QString getFileName() const;

    QString getShowName() const;
    void setShowName(const QString& showName);

public slots:
    bool load(const QString& f);
    bool fileSave();
    void setCurrentFileName(const QString& fileName);
    void saveFileAsBinary(QDataStream& data);
    void readFromBinary(QDataStream& data);
    void fileNew();

signals:
    void showed(bool);
    void fileNameChanged(QString);

protected:
    virtual void closeEvent(QCloseEvent* e);
    virtual void showEvent(QShowEvent* e);
    virtual void hideEvent(QHideEvent* e);

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();

    bool maybeSave();

private slots:
    void fileOpen();

    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();
    void drawDoc();
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString& f);
    void textSize(const QString& p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction* a);
    void onRead(int now, int tot);
    void currentCharFormatChanged(const QTextCharFormat& format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    void printPreview(QPrinter*);

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat& format);
    void fontChanged(const QFont& f);
    void colorChanged(const QColor& c);
    void alignmentChanged(Qt::Alignment a);

private:
    QPointer<NoteController> m_noteCtrl;

    QAction *actionSave, *actionTextBold, *actionTextUnderline, *actionTextItalic, *actionTextColor, *actionAlignLeft,
        *actionAlignCenter, *actionAlignRight, *actionAlignJustify, *actionUndo, *actionRedo, *actionCut, *actionCopy,
        *actionPaste;

    QStatusBar* statusbar;
    OOReader* Ooo;
    PushDoc* force;
    QSettings setter;
    QComboBox* comboStyle;
    QFontComboBox* comboFont;
    QComboBox* comboSize;

    QToolBar* tb;
    QString fileName;
    QTextEdit* textEdit;

    static const QString rsrcPath;
};

#endif
