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

#include "textedit.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QStatusBar>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextList>
#include <QToolBar>
#include <QtDebug>

#include "controller/view_controller/notecontroller.h"

#ifdef Q_WS_MAC
const QString TextEdit::rsrcPath= QStringLiteral(":/images/mac");
#else
const QString TextEdit::rsrcPath= QStringLiteral(":/images/win");
#endif

TextEdit::TextEdit(NoteController* note, QWidget* parent) : QMainWindow(parent), m_noteCtrl(note)
{
    setObjectName("TextEdit");
    setupFileActions();
    setupEditActions();
    setupTextActions();

    // connect(m_noteCtrl, &NoteController::textChanged, this, &TextEdit::setCurrentFileName);
    connect(m_noteCtrl, &NoteController::pathChanged, this, &TextEdit::load);

    //    {
    //        QMenu *helpMenu = new QMenu(tr("Help"), this);
    //        menuBar()->addMenu(helpMenu);
    //        helpMenu->addAction(tr("About"), this, SLOT(about()));
    //        helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
    //    }

    textEdit= new QTextEdit(this);
    connect(textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this,
            SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

    setCentralWidget(textEdit);
    textEdit->setFocus();
    setCurrentFileName(m_noteCtrl->name());

    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    connect(textEdit->document(), &QTextDocument::contentsChanged, m_noteCtrl,
            [this]() { m_noteCtrl->setText(textEdit->document()->toHtml()); });

    // connect(textEdit->document(), SIGNAL(modificationChanged(bool)), actionSave, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
    connect(textEdit->document(), SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));

    setWindowModified(textEdit->document()->isModified());
    // actionSave->setEnabled(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

    connect(actionUndo, SIGNAL(triggered()), textEdit, SLOT(undo()));
    connect(actionRedo, SIGNAL(triggered()), textEdit, SLOT(redo()));

    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);

    connect(actionCut, SIGNAL(triggered()), textEdit, SLOT(cut()));
    connect(actionCopy, SIGNAL(triggered()), textEdit, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), textEdit, SLOT(paste()));

    connect(textEdit, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

    statusbar= new QStatusBar(this);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    setStatusBar(statusbar);

    auto p= m_noteCtrl->text();
    if(!load(m_noteCtrl->path()) && p.isEmpty())
        fileNew();
    else
        textEdit->setHtml(p);
}

void TextEdit::closeEvent(QCloseEvent* e)
{
    if(maybeSave())
    {
        e->accept();
        emit showed(false);
    }
    else
        e->ignore();
}
void TextEdit::showEvent(QShowEvent* e)
{
    emit showed(true);
    QMainWindow::showEvent(e);
}
void TextEdit::hideEvent(QHideEvent* e)
{
    emit showed(false);
    QMainWindow::hideEvent(e);
}

void TextEdit::setupFileActions()
{
    QToolBar* tb= new QToolBar(this);
    tb->setWindowTitle(tr("File Actions"));
    addToolBar(tb);

    QMenu* menu= new QMenu(tr("&File"), this);
    menuBar()->addMenu(menu);

    QAction* a;

    /*a= new QAction(QIcon(rsrcPath + "/filenew.png"), tr("&New"), this);
    a->setShortcut(QKeySequence::New);
    connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    tb->addAction(a);
    menu->addAction(a);

    a= new QAction(QIcon(rsrcPath + "/fileopen.png"), tr("&Open..."), this);
    a->setShortcut(QKeySequence::Open);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    tb->addAction(a);
    menu->addAction(a);*/

    // menu->addSeparator();

    /**actionSave= a= new QAction(QIcon(rsrcPath + "/filesave.png"), tr("&Save"), this);
    a->setShortcut(QKeySequence::Save);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);*/

    /*a= new QAction(tr("Save &As..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    menu->addAction(a);
    menu->addSeparator();*/

    a= new QAction(QIcon::fromTheme("fileprint"), tr("&Print..."), this);
    a->setShortcut(QKeySequence::Print);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
    tb->addAction(a);
    menu->addAction(a);

    a= new QAction(QIcon::fromTheme("fileprint"), tr("Print Preview..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
    menu->addAction(a);

    a= new QAction(QIcon::fromTheme("exportpdf"), tr("&Export PDF..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
    tb->addAction(a);
    menu->addAction(a);

    // menu->addSeparator();

    /*a= new QAction(tr("&Quit"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(a, SIGNAL(triggered()), this, SLOT(hide()));
    menu->addAction(a);*/
}

void TextEdit::setupEditActions()
{
    QToolBar* tb= new QToolBar(this);
    tb->setWindowTitle(tr("Edit Actions"));
    addToolBar(tb);

    QMenu* menu= new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(menu);

    QAction* a;
    a= actionUndo= new QAction(QIcon::fromTheme("editundo"), tr("&Undo"), this);
    a->setShortcut(QKeySequence::Undo);
    tb->addAction(a);
    menu->addAction(a);
    a= actionRedo= new QAction(QIcon::fromTheme("editredo"), tr("&Redo"), this);
    a->setShortcut(QKeySequence::Redo);
    tb->addAction(a);
    menu->addAction(a);
    menu->addSeparator();
    a= actionCut= new QAction(QIcon::fromTheme("editcut"), tr("Cu&t"), this);
    a->setShortcut(QKeySequence::Cut);
    tb->addAction(a);
    menu->addAction(a);
    a= actionCopy= new QAction(QIcon::fromTheme("editcopy"), tr("&Copy"), this);
    a->setShortcut(QKeySequence::Copy);
    tb->addAction(a);
    menu->addAction(a);
    a= actionPaste= new QAction(QIcon::fromTheme("editpaste"), tr("&Paste"), this);
    a->setShortcut(QKeySequence::Paste);
    tb->addAction(a);
    menu->addAction(a);
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void TextEdit::setupTextActions()
{
    QToolBar* tb= new QToolBar(this);
    tb->setWindowTitle(tr("Format Actions"));
    addToolBar(tb);

    QMenu* menu= new QMenu(tr("F&ormat"), this);
    menuBar()->addMenu(menu);

    actionTextBold= new QAction(QIcon(rsrcPath + "/textbold.png"), tr("&Bold"), this);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
    tb->addAction(actionTextBold);
    menu->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    actionTextItalic= new QAction(QIcon(rsrcPath + "/textitalic.png"), tr("&Italic"), this);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
    tb->addAction(actionTextItalic);
    menu->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    actionTextUnderline= new QAction(QIcon(rsrcPath + "/textunder.png"), tr("&Underline"), this);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
    tb->addAction(actionTextUnderline);
    menu->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    QActionGroup* grp= new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    actionAlignLeft= new QAction(QIcon::fromTheme("textleft"), tr("&Left"), grp);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignCenter= new QAction(QIcon::fromTheme("textcenter"), tr("C&enter"), grp);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignRight= new QAction(QIcon::fromTheme("textright"), tr("&Right"), grp);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignJustify= new QAction(QIcon::fromTheme("textjustify"), tr("&Justify"), grp);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);

    tb->addActions(grp->actions());
    menu->addActions(grp->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor= new QAction(pix, tr("&Color..."), this);
    connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
    tb->addAction(actionTextColor);
    menu->addAction(actionTextColor);

    tb= new QToolBar(this);
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    tb->setWindowTitle(tr("Format Actions"));
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    comboStyle= new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    comboFont= new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

    comboSize= new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    QFontDatabase db;
    for(int& size : db.standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));
}

void TextEdit::drawDoc()
{
    /* remote image is loading after ..... */
    if(Ooo)
    {
        textEdit->setDocument(Ooo->document()->clone());
        delete Ooo;
    }

    if(force)
    {
        force->quit();
    }
}

void TextEdit::onRead(int, int)
{
    //////qDebug() << "### reading line -> " << now <<  " t." << tot;
}

bool TextEdit::load(const QString& f)
{
    QFileInfo fi(f);
    const QString ext= fi.completeSuffix().toLower();
    if(!fi.exists())
    {
        return false;
    }

    if(ext == "odt" || ext == "ott")
    {
        force= new PushDoc(this);
        Ooo= new OOReader(f);
        Ooo->moveToThread(force);
        connect(force, SIGNAL(started()), Ooo, SLOT(read()));
        connect(Ooo, SIGNAL(ready()), this, SLOT(drawDoc()));
        connect(Ooo, SIGNAL(statusRead(int, int)), this, SLOT(onRead(int, int)));
        force->start();

        setCurrentFileName(f);
        return true;
    }
    else
    {
        QFile file(f);
        if(!file.open(QFile::ReadOnly))
            return false;

        QByteArray data= file.readAll();
        QTextCodec* codec= Qt::codecForHtml(data);
        QString str= codec->toUnicode(data);
        if(Qt::mightBeRichText(str))
        {
            textEdit->setHtml(str);
        }
        else
        {
            str= QString::fromLocal8Bit(data);
            textEdit->setPlainText(str);
        }

        setCurrentFileName(f);
        return true;
    }
}

bool TextEdit::maybeSave()
{
    if(!textEdit->document()->isModified())
        return true;
    if(fileName.startsWith(QLatin1String(":/")))
        return true;
    QMessageBox::StandardButton ret;
    ret= QMessageBox::warning(this, tr("Application"),
                              tr("The document has been modified.\n"
                                 "Do you want to save your changes?"),
                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if(ret == QMessageBox::Save)
        return fileSave();
    else if(ret == QMessageBox::Cancel)
        return false;
    return true;
}

void TextEdit::setCurrentFileName(const QString& fileName)
{
    this->fileName= fileName;
    textEdit->document()->setModified(false);
    emit fileNameChanged(fileName);
}

void TextEdit::fileNew()
{
    if(maybeSave())
    {
        textEdit->clear();
        setCurrentFileName(QString());
    }
}

void TextEdit::fileOpen()
{
    QString fn= QFileDialog::getOpenFileName(this, tr("Open File..."), QString(setter.value("LastDir").toString()),
                                             getFilter());
    if(!fn.isEmpty())
    {
        QFileInfo fi(fn);
        setter.setValue("LastDir", fi.absolutePath() + "/");
        load(fn);
    }
}

bool TextEdit::fileSave()
{
    if(fileName.isEmpty())
        return fileSaveAs();

    bool canodt= true;
    const QString ext= QFileInfo(fileName).completeSuffix().toLower();
    if(ext == "odt" && canodt)
    {
        QTextDocumentWriter writer(fileName);
        return writer.write(textEdit->document());
    }
    else
    {
        QFile file(fileName);
        if(!file.open(QFile::WriteOnly))
            return false;
        QTextStream ts(&file);
        ts.setCodec(QTextCodec::codecForName("UTF-8"));
        ts << textEdit->document()->toHtml("UTF-8");
        textEdit->document()->setModified(false);
    }
    return true;
}
void TextEdit::saveFileAsBinary(QDataStream& data)
{
    data << textEdit->document()->toHtml("UTF-8");
    textEdit->document()->setModified(false);
}
void TextEdit::readFromBinary(QDataStream& data)
{
    QString str;
    data >> str;
    if(Qt::mightBeRichText(str))
    {
        textEdit->setHtml(str);
    }
    else
    {
        textEdit->setPlainText(str);
    }
}

bool TextEdit::fileSaveAs()
{
    QString support;
#if QT_VERSION >= 0x040500
    support= tr("ODF files (*.odt);;HTML-Files (*.htm *.html);;All Files (*)");
#else
    support= tr("HTML-Files (*.htm *.html);;All Files (*)");
#endif
    QString fn= QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), support);

    if(fn.isEmpty())
        return false;
    setCurrentFileName(fn);
    return fileSave();
}

void TextEdit::filePrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog* dlg= new QPrintDialog(&printer, this);
    if(textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if(dlg->exec() == QDialog::Accepted)
    {
        textEdit->print(&printer);
    }
    delete dlg;
#endif
}

void TextEdit::filePrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags(Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void TextEdit::printPreview(QPrinter* printer)
{
#ifndef QT_NO_PRINTER
    textEdit->print(printer);
#endif
}

void TextEdit::filePrintPdf()
{
#ifndef QT_NO_PRINTER
    //! [0]
    QString fileName= QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
    if(!fileName.isEmpty())
    {
        if(QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        textEdit->document()->print(&printer);
    }
    //! [0]
#endif
}

void TextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString& f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString& p)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(p.toFloat());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textStyle(int styleIndex)
{
    QTextCursor cursor= textEdit->textCursor();

    if(styleIndex != 0)
    {
        QTextListFormat::Style style= QTextListFormat::ListDisc;

        switch(styleIndex)
        {
        default:
        case 1:
            style= QTextListFormat::ListDisc;
            break;
        case 2:
            style= QTextListFormat::ListCircle;
            break;
        case 3:
            style= QTextListFormat::ListSquare;
            break;
        case 4:
            style= QTextListFormat::ListDecimal;
            break;
        case 5:
            style= QTextListFormat::ListLowerAlpha;
            break;
        case 6:
            style= QTextListFormat::ListUpperAlpha;
            break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt= cursor.blockFormat();

        QTextListFormat listFmt;

        if(cursor.currentList())
        {
            listFmt= cursor.currentList()->format();
        }
        else
        {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    }
    else
    {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void TextEdit::textColor()
{
    QColor col= QColorDialog::getColor(textEdit->textColor(), this);
    if(!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEdit::textAlign(QAction* a)
{
    if(a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft);
    else if(a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if(a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight);
    else if(a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat& format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TextEdit::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
}

void TextEdit::clipboardDataChanged()
{
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void TextEdit::about()
{
    QMessageBox::about(this, tr("About"),
                       tr("This example demonstrates Qt's "
                          "rich text editing facilities in action, providing an example "
                          "document for you to experiment with."));
}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
    QTextCursor cursor= textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::fontChanged(const QFont& f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor& c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void TextEdit::alignmentChanged(Qt::Alignment a)
{
    if(a & Qt::AlignLeft)
    {
        actionAlignLeft->setChecked(true);
    }
    else if(a & Qt::AlignHCenter)
    {
        actionAlignCenter->setChecked(true);
    }
    else if(a & Qt::AlignRight)
    {
        actionAlignRight->setChecked(true);
    }
    else if(a & Qt::AlignJustify)
    {
        actionAlignJustify->setChecked(true);
    }
}

QString TextEdit::getFileName() const
{
    return fileName;
}

QString TextEdit::getFilter() const
{
    return QString("%1 %2 %3 %4")
        .arg(tr("OpenOffice 2.4 file format OASIS "), QStringLiteral(" (*.odt *.ott);;"),
             QStringLiteral("XHTML file format"), QStringLiteral(" (*.htm *.html);;"));
}
