/*
** Copyright (C) 2013 Jiří Procházka (Hobrasoft)
** Contact: http://www.hobrasoft.cz/
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file is under the terms of the GNU Lesser General Public License
** version 2.1 as published by the Free Software Foundation and appearing
** in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the
** GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
*/

#include "mrichtextedit.h"
#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QColorDialog>
#include <QDialog>
#include <QFileDialog>
#include <QFontDatabase>
#include <QImageReader>
#include <QInputDialog>
#include <QMenu>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QSettings>
#include <QTextList>
#include <QUrl>
#include <QtDebug>

#include "ui_mrichtextedit.h"

MRichTextEdit::MRichTextEdit(QWidget* parent) : QWidget(parent), m_ui(new Ui::MRichTextEdit)
{
    m_ui->setupUi(this);
    m_lastBlockList= nullptr;
    m_ui->f_textedit->setTabStopDistance(40);

    connect(m_ui->f_textedit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this,
            SLOT(slotCurrentCharFormatChanged(QTextCharFormat)));
    connect(m_ui->f_textedit, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));

    m_fontsize_h1= 18;
    m_fontsize_h2= 16;
    m_fontsize_h3= 14;
    m_fontsize_h4= 12;

    fontChanged(m_ui->f_textedit->font());
    bgColorChanged(m_ui->f_textedit->textColor());

    // paragraph formatting

    m_paragraphItems << tr("Standard") << tr("Heading 1") << tr("Heading 2") << tr("Heading 3") << tr("Heading 4")
                     << tr("Monospace");
    m_ui->f_paragraph->addItems(m_paragraphItems);

    connect(m_ui->f_paragraph, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    // undo & redo

    m_ui->f_undo->setShortcut(QKeySequence::Undo);
    m_ui->f_redo->setShortcut(QKeySequence::Redo);

    connect(m_ui->f_textedit->document(), SIGNAL(undoAvailable(bool)), m_ui->f_undo, SLOT(setEnabled(bool)));
    connect(m_ui->f_textedit->document(), SIGNAL(redoAvailable(bool)), m_ui->f_redo, SLOT(setEnabled(bool)));

    m_ui->f_undo->setEnabled(m_ui->f_textedit->document()->isUndoAvailable());
    m_ui->f_redo->setEnabled(m_ui->f_textedit->document()->isRedoAvailable());

    connect(m_ui->f_undo, SIGNAL(clicked()), m_ui->f_textedit, SLOT(undo()));
    connect(m_ui->f_redo, SIGNAL(clicked()), m_ui->f_textedit, SLOT(redo()));

    // cut, copy & paste

    m_ui->f_cut->setShortcut(QKeySequence::Cut);
    m_ui->f_copy->setShortcut(QKeySequence::Copy);
    m_ui->f_paste->setShortcut(QKeySequence::Paste);

    m_ui->f_cut->setEnabled(false);
    m_ui->f_copy->setEnabled(false);

    connect(m_ui->f_cut, SIGNAL(clicked()), m_ui->f_textedit, SLOT(cut()));
    connect(m_ui->f_copy, SIGNAL(clicked()), m_ui->f_textedit, SLOT(copy()));
    connect(m_ui->f_paste, SIGNAL(clicked()), m_ui->f_textedit, SLOT(paste()));

    connect(m_ui->f_textedit, SIGNAL(copyAvailable(bool)), m_ui->f_cut, SLOT(setEnabled(bool)));
    connect(m_ui->f_textedit, SIGNAL(copyAvailable(bool)), m_ui->f_copy, SLOT(setEnabled(bool)));

#ifndef QT_NO_CLIPBOARD
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(slotClipboardDataChanged()));
#endif

    // link

    m_ui->f_link->setShortcut(Qt::CTRL + Qt::Key_L);

    connect(m_ui->f_link, SIGNAL(clicked(bool)), this, SLOT(textLink(bool)));

    // bold, italic & underline

    m_ui->f_bold->setShortcut(Qt::CTRL + Qt::Key_B);
    m_ui->f_italic->setShortcut(Qt::CTRL + Qt::Key_I);
    m_ui->f_underline->setShortcut(Qt::CTRL + Qt::Key_U);

    connect(m_ui->f_bold, SIGNAL(clicked()), this, SLOT(textBold()));
    connect(m_ui->f_italic, SIGNAL(clicked()), this, SLOT(textItalic()));
    connect(m_ui->f_underline, SIGNAL(clicked()), this, SLOT(textUnderline()));
    connect(m_ui->f_strikeout, SIGNAL(clicked()), this, SLOT(textStrikeout()));

    QAction* removeFormat= new QAction(tr("Remove character formatting"), this);
    removeFormat->setShortcut(QKeySequence("CTRL+M"));
    connect(removeFormat, SIGNAL(triggered()), this, SLOT(textRemoveFormat()));
    m_ui->f_textedit->addAction(removeFormat);

    QAction* removeAllFormat= new QAction(tr("Remove all formatting"), this);
    connect(removeAllFormat, SIGNAL(triggered()), this, SLOT(textRemoveAllFormat()));
    m_ui->f_textedit->addAction(removeAllFormat);

    QAction* textsource= new QAction(tr("Edit document source"), this);
    textsource->setShortcut(QKeySequence("CTRL+O"));
    connect(textsource, SIGNAL(triggered()), this, SLOT(textSource()));
    m_ui->f_textedit->addAction(textsource);

    QMenu* menu= new QMenu(this);
    menu->addAction(removeAllFormat);
    menu->addAction(removeFormat);
    menu->addAction(textsource);
    m_ui->f_menu->setMenu(menu);
    m_ui->f_menu->setPopupMode(QToolButton::InstantPopup);

    // lists

    m_ui->f_list_bullet->setShortcut(Qt::CTRL + Qt::Key_Minus);
    m_ui->f_list_ordered->setShortcut(Qt::CTRL + Qt::Key_Equal);

    connect(m_ui->f_list_bullet, SIGNAL(clicked(bool)), this, SLOT(listBullet(bool)));
    connect(m_ui->f_list_ordered, SIGNAL(clicked(bool)), this, SLOT(listOrdered(bool)));

    // indentation

    m_ui->f_indent_dec->setShortcut(Qt::CTRL + Qt::Key_Comma);
    m_ui->f_indent_inc->setShortcut(Qt::CTRL + Qt::Key_Period);

    connect(m_ui->f_indent_inc, SIGNAL(clicked()), this, SLOT(increaseIndentation()));
    connect(m_ui->f_indent_dec, SIGNAL(clicked()), this, SLOT(decreaseIndentation()));

    // font size

    QFontDatabase db;
    for(int& size : db.standardSizes())
        m_ui->f_fontsize->addItem(QString::number(size));

    connect(m_ui->f_fontsize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    m_ui->f_fontsize->setCurrentIndex(m_ui->f_fontsize->findText(QString::number(QApplication::font().pointSize())));

    // text foreground color

    QPixmap pix(16, 16);
    pix.fill(QApplication::palette().windowText().color());
    m_ui->f_fgcolor->setIcon(pix);

    connect(m_ui->f_fgcolor, SIGNAL(clicked()), this, SLOT(textFgColor()));

    // text background color

    pix.fill(QApplication::palette().window().color());
    m_ui->f_bgcolor->setIcon(pix);

    connect(m_ui->f_bgcolor, SIGNAL(clicked()), this, SLOT(textBgColor()));

    // images
    connect(m_ui->f_image, SIGNAL(clicked()), this, SLOT(insertImage()));
}

MRichTextEdit::~MRichTextEdit() = default;

QString MRichTextEdit::toPlainText() const { return m_ui->f_textedit->toPlainText(); }

void MRichTextEdit::textSource()
{
    QDialog* dialog= new QDialog(this);
    QPlainTextEdit* pte= new QPlainTextEdit(dialog);
    pte->setPlainText(m_ui->f_textedit->toHtml());
    QGridLayout* gl= new QGridLayout(dialog);
    gl->addWidget(pte, 0, 0, 1, 1);
    dialog->setWindowTitle(tr("Document source"));
    dialog->setMinimumWidth(400);
    dialog->setMinimumHeight(600);
    dialog->exec();

    m_ui->f_textedit->setHtml(pte->toPlainText());

    delete dialog;
}

void MRichTextEdit::textRemoveFormat()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Normal);
    fmt.setFontUnderline(false);
    fmt.setFontStrikeOut(false);
    fmt.setFontItalic(false);
    fmt.setFontPointSize(9);
    //  fmt.setFontFamily     ("Helvetica");
    //  fmt.setFontStyleHint  (QFont::SansSerif);
    //  fmt.setFontFixedPitch (true);

    m_ui->f_bold->setChecked(false);
    m_ui->f_underline->setChecked(false);
    m_ui->f_italic->setChecked(false);
    m_ui->f_strikeout->setChecked(false);
    m_ui->f_fontsize->setCurrentIndex(m_ui->f_fontsize->findText("9"));

    //  QTextBlockFormat bfmt = cursor.blockFormat();
    //  bfmt->setIndent(0);

    fmt.clearBackground();

    mergeFormatOnWordOrSelection(fmt);
}

void MRichTextEdit::textRemoveAllFormat()
{
    m_ui->f_bold->setChecked(false);
    m_ui->f_underline->setChecked(false);
    m_ui->f_italic->setChecked(false);
    m_ui->f_strikeout->setChecked(false);
    m_ui->f_fontsize->setCurrentIndex(m_ui->f_fontsize->findText("9"));
    QString text= m_ui->f_textedit->toPlainText();
    m_ui->f_textedit->setPlainText(text);
}

void MRichTextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(m_ui->f_bold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void MRichTextEdit::focusInEvent(QFocusEvent*)
{
    m_ui->f_textedit->setFocus(Qt::TabFocusReason);
}

void MRichTextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(m_ui->f_underline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MRichTextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(m_ui->f_italic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MRichTextEdit::textStrikeout()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(m_ui->f_strikeout->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MRichTextEdit::textSize(const QString& p)
{
    qreal pointSize= p.toFloat();
    if(p.toFloat() > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void MRichTextEdit::textLink(bool checked)
{
    bool unlink= false;
    QTextCharFormat fmt;
    if(checked)
    {
        QString url= m_ui->f_textedit->currentCharFormat().anchorHref();
        bool ok;
        QString newUrl= QInputDialog::getText(this, tr("Create a link"), tr("Link URL:"), QLineEdit::Normal, url, &ok);
        if(ok)
        {
            fmt.setAnchor(true);
            fmt.setAnchorHref(newUrl);
            fmt.setForeground(QApplication::palette().color(QPalette::Link));
            fmt.setFontUnderline(true);
        }
        else
        {
            unlink= true;
        }
    }
    else
    {
        unlink= true;
    }
    if(unlink)
    {
        fmt.setAnchor(false);
        fmt.setForeground(QApplication::palette().color(QPalette::Text));
        fmt.setFontUnderline(false);
    }
    mergeFormatOnWordOrSelection(fmt);
}

void MRichTextEdit::textStyle(int index)
{
    QTextCursor cursor= m_ui->f_textedit->textCursor();
    cursor.beginEditBlock();

    // standard
    if(!cursor.hasSelection())
    {
        cursor.select(QTextCursor::BlockUnderCursor);
    }
    QTextCharFormat fmt;
    cursor.setCharFormat(fmt);
    m_ui->f_textedit->setCurrentCharFormat(fmt);

    if(index == ParagraphHeading1 || index == ParagraphHeading2 || index == ParagraphHeading3
       || index == ParagraphHeading4)
    {
        if(index == ParagraphHeading1)
        {
            fmt.setFontPointSize(m_fontsize_h1);
        }
        if(index == ParagraphHeading2)
        {
            fmt.setFontPointSize(m_fontsize_h2);
        }
        if(index == ParagraphHeading3)
        {
            fmt.setFontPointSize(m_fontsize_h3);
        }
        if(index == ParagraphHeading4)
        {
            fmt.setFontPointSize(m_fontsize_h4);
        }
        if(index == ParagraphHeading2 || index == ParagraphHeading4)
        {
            fmt.setFontItalic(true);
        }

        fmt.setFontWeight(QFont::Bold);
    }
    if(index == ParagraphMonospace)
    {
        fmt= cursor.charFormat();
        fmt.setFontFamily("Monospace");
        fmt.setFontStyleHint(QFont::Monospace);
        fmt.setFontFixedPitch(true);
    }
    cursor.setCharFormat(fmt);
    m_ui->f_textedit->setCurrentCharFormat(fmt);

    cursor.endEditBlock();
}

void MRichTextEdit::textFgColor()
{
    QColor col= QColorDialog::getColor(m_ui->f_textedit->textColor(), this);
    QTextCursor cursor= m_ui->f_textedit->textCursor();
    if(!cursor.hasSelection())
    {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    QTextCharFormat fmt= cursor.charFormat();
    if(col.isValid())
    {
        fmt.setForeground(col);
    }
    else
    {
        fmt.clearForeground();
    }
    cursor.setCharFormat(fmt);
    m_ui->f_textedit->setCurrentCharFormat(fmt);
    fgColorChanged(col);
}

void MRichTextEdit::textBgColor()
{
    QColor col= QColorDialog::getColor(m_ui->f_textedit->textBackgroundColor(), this);
    QTextCursor cursor= m_ui->f_textedit->textCursor();
    if(!cursor.hasSelection())
    {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    QTextCharFormat fmt= cursor.charFormat();
    if(col.isValid())
    {
        fmt.setBackground(col);
    }
    else
    {
        fmt.clearBackground();
    }
    cursor.setCharFormat(fmt);
    m_ui->f_textedit->setCurrentCharFormat(fmt);
    bgColorChanged(col);
}

void MRichTextEdit::listBullet(bool checked)
{
    if(checked)
    {
        m_ui->f_list_ordered->setChecked(false);
    }
    list(checked, QTextListFormat::ListDisc);
}

void MRichTextEdit::listOrdered(bool checked)
{
    if(checked)
    {
        m_ui->f_list_bullet->setChecked(false);
    }
    list(checked, QTextListFormat::ListDecimal);
}

void MRichTextEdit::list(bool checked, QTextListFormat::Style style)
{
    QTextCursor cursor= m_ui->f_textedit->textCursor();
    cursor.beginEditBlock();
    if(!checked)
    {
        QTextBlockFormat obfmt= cursor.blockFormat();
        QTextBlockFormat bfmt;
        bfmt.setIndent(obfmt.indent());
        cursor.setBlockFormat(bfmt);
    }
    else
    {
        QTextListFormat listFmt;
        if(cursor.currentList())
        {
            listFmt= cursor.currentList()->format();
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }
    cursor.endEditBlock();
}

void MRichTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
    QTextCursor cursor= m_ui->f_textedit->textCursor();
    if(!cursor.hasSelection())
    {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    cursor.mergeCharFormat(format);
    m_ui->f_textedit->mergeCurrentCharFormat(format);
    m_ui->f_textedit->setFocus(Qt::TabFocusReason);
}

void MRichTextEdit::slotCursorPositionChanged()
{
    QTextList* l= m_ui->f_textedit->textCursor().currentList();
    if(m_lastBlockList
       && (l == m_lastBlockList
           || (l != nullptr && m_lastBlockList != nullptr && l->format().style() == m_lastBlockList->format().style())))
    {
        return;
    }
    m_lastBlockList= l;
    if(l)
    {
        QTextListFormat lfmt= l->format();
        if(lfmt.style() == QTextListFormat::ListDisc)
        {
            m_ui->f_list_bullet->setChecked(true);
            m_ui->f_list_ordered->setChecked(false);
        }
        else if(lfmt.style() == QTextListFormat::ListDecimal)
        {
            m_ui->f_list_bullet->setChecked(false);
            m_ui->f_list_ordered->setChecked(true);
        }
        else
        {
            m_ui->f_list_bullet->setChecked(false);
            m_ui->f_list_ordered->setChecked(false);
        }
    }
    else
    {
        m_ui->f_list_bullet->setChecked(false);
        m_ui->f_list_ordered->setChecked(false);
    }
}

void MRichTextEdit::fontChanged(const QFont& f)
{
    m_ui->f_fontsize->setCurrentIndex(m_ui->f_fontsize->findText(QString::number(f.pointSize())));
    m_ui->f_bold->setChecked(f.bold());
    m_ui->f_italic->setChecked(f.italic());
    m_ui->f_underline->setChecked(f.underline());
    m_ui->f_strikeout->setChecked(f.strikeOut());
    if(f.pointSize() == m_fontsize_h1)
    {
        m_ui->f_paragraph->setCurrentIndex(ParagraphHeading1);
    }
    else if(f.pointSize() == m_fontsize_h2)
    {
        m_ui->f_paragraph->setCurrentIndex(ParagraphHeading2);
    }
    else if(f.pointSize() == m_fontsize_h3)
    {
        m_ui->f_paragraph->setCurrentIndex(ParagraphHeading3);
    }
    else if(f.pointSize() == m_fontsize_h4)
    {
        m_ui->f_paragraph->setCurrentIndex(ParagraphHeading4);
    }
    else
    {
        if(f.fixedPitch() && f.family() == "Monospace")
        {
            m_ui->f_paragraph->setCurrentIndex(ParagraphMonospace);
        }
        else
        {
            m_ui->f_paragraph->setCurrentIndex(ParagraphStandard);
        }
    }
    if(m_ui->f_textedit->textCursor().currentList())
    {
        QTextListFormat lfmt= m_ui->f_textedit->textCursor().currentList()->format();
        if(lfmt.style() == QTextListFormat::ListDisc)
        {
            m_ui->f_list_bullet->setChecked(true);
            m_ui->f_list_ordered->setChecked(false);
        }
        else if(lfmt.style() == QTextListFormat::ListDecimal)
        {
            m_ui->f_list_bullet->setChecked(false);
            m_ui->f_list_ordered->setChecked(true);
        }
        else
        {
            m_ui->f_list_bullet->setChecked(false);
            m_ui->f_list_ordered->setChecked(false);
        }
    }
    else
    {
        m_ui->f_list_bullet->setChecked(false);
        m_ui->f_list_ordered->setChecked(false);
    }
}

void MRichTextEdit::fgColorChanged(const QColor& c)
{
    QPixmap pix(16, 16);
    if(c.isValid())
    {
        pix.fill(c);
    }
    else
    {
        pix.fill(QApplication::palette().windowText().color());
    }
    m_ui->f_fgcolor->setIcon(pix);
}

void MRichTextEdit::bgColorChanged(const QColor& c)
{
    QPixmap pix(16, 16);
    if(c.isValid())
    {
        pix.fill(c);
    }
    else
    {
        pix.fill(QApplication::palette().window().color());
    }
    m_ui->f_bgcolor->setIcon(pix);
}

void MRichTextEdit::slotCurrentCharFormatChanged(const QTextCharFormat& format)
{
    fontChanged(format.font());
    bgColorChanged((format.background().isOpaque()) ? format.background().color() : QColor());
    fgColorChanged((format.foreground().isOpaque()) ? format.foreground().color() : QColor());
    m_ui->f_link->setChecked(format.isAnchor());
}

void MRichTextEdit::slotClipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if(const QMimeData* md= QApplication::clipboard()->mimeData())
        m_ui->f_paste->setEnabled(md->hasText());
#endif
}

QString MRichTextEdit::toHtml() const
{
    QString s= m_ui->f_textedit->toHtml();
    // convert emails to links
    s= s.replace(QRegularExpression("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)([a-zA-Z\\d]+@[a-zA-Z\\d]+\\.[a-zA-Z]+)"),
                 "\\1<a href=\"mailto:\\2\">\\2</a>");
    // convert links
    s= s.replace(QRegularExpression("(<[^a][^>]+>(?:<span[^>]+>)?|\\s)((?:https?|ftp|file)://[^\\s'\"<>]+)"),
                 "\\1<a href=\"\\2\">\\2</a>");
    // see also: Utils::linkify()
    return s;
}

QTextDocument *MRichTextEdit::document() { return m_ui->f_textedit->document(); }

QTextCursor MRichTextEdit::textCursor() const { return m_ui->f_textedit->textCursor(); }

void MRichTextEdit::setTextCursor(const QTextCursor &cursor) { m_ui->f_textedit->setTextCursor(cursor); }

void MRichTextEdit::increaseIndentation()
{
    indent(+1);
}

void MRichTextEdit::decreaseIndentation()
{
    indent(-1);
}

void MRichTextEdit::indent(int delta)
{
    QTextCursor cursor= m_ui->f_textedit->textCursor();
    cursor.beginEditBlock();
    QTextBlockFormat bfmt= cursor.blockFormat();
    int ind= bfmt.indent();
    if(ind + delta >= 0)
    {
        bfmt.setIndent(ind + delta);
    }
    cursor.setBlockFormat(bfmt);
    cursor.endEditBlock();
}

void MRichTextEdit::setText(const QString& text)
{
    if(text.isEmpty())
    {
        setPlainText(text);
        return;
    }
    if(text[0] == '<')
    {
        setHtml(text);
    }
    else
    {
        setPlainText(text);
    }
}

void MRichTextEdit::setPlainText(const QString &text) { m_ui->f_textedit->setPlainText(text); }

void MRichTextEdit::setHtml(const QString &text) { m_ui->f_textedit->setHtml(text); }

void MRichTextEdit::insertImage()
{
    QSettings s;
    QString attdir= s.value("general/filedialog-path").toString();
    QString file= QFileDialog::getOpenFileName(this, tr("Select an image"), attdir,
                                               tr("JPEG (*.jpg);; GIF (*.gif);; PNG (*.png);; BMP (*.bmp);; All (*)"));
    QImage image= QImageReader(file).read();

    m_ui->f_textedit->dropImage(image, QFileInfo(file).suffix().toUpper().toLocal8Bit().data());
}
