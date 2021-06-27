/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "document.h"

#include <QDebug>
#include <QDialog>
#include <QFontMetrics>
#include <QLayout>
#include <QMessageBox>
#include <QRegExp>
#include <QTextDocumentFragment>

#include "controller/view_controller/sharednotecontroller.h"
#include "markdownhighlighter.h"
#include "ui_document.h"

#include "enu.h"
#include "utilities.h"

Document::Document(SharedNoteController* ctrl, QWidget* parent)
    : QWidget(parent), m_shareCtrl(ctrl), ui(new Ui::Document), m_highlighter(nullptr)
{
    ui->setupUi(this);

    // Set up the editor
    delete ui->editorFrame;
    m_editor= new CodeEditor(m_shareCtrl, this);

    if(m_shareCtrl)
        setPlainText(m_shareCtrl->text());

    m_previewMarkdown= new QTextEdit();
    m_previewMarkdown->setReadOnly(true);
    QFontMetricsF fm(m_editor->font());
    m_editor->setTabStopDistance(fm.averageCharWidth() * 4.);
    ui->editorSplitter->insertWidget(0, m_editor);

    m_previewMarkdown->setVisible(m_shareCtrl->markdownVisible());

    m_participantPane.reset(new ParticipantsPane(m_shareCtrl));
    ui->participantSplitter->insertWidget(1, m_previewMarkdown);
    ui->participantSplitter->insertWidget(2, m_participantPane.get());

    connect(m_shareCtrl, &SharedNoteController::permissionChanged, this, [this](ParticipantModel::Permission perm) {
        m_editor->setReadOnly(ParticipantModel::Permission::readOnly == perm);
    });

    connect(m_shareCtrl, &SharedNoteController::markdownVisibleChanged, m_previewMarkdown, &QTextEdit::setVisible);
    connect(m_shareCtrl, &SharedNoteController::textChanged, m_previewMarkdown, &QTextEdit::setMarkdown);

    connect(m_editor, &CodeEditor::textChanged, this, &Document::contentChanged);

    m_editor->setReadOnly(m_shareCtrl->permission() == ParticipantModel::Permission::readOnly);

    // delete ui->findAllFrame;
    findAllToolbar= new FindToolBar(this);
    ui->editorVerticalLayout->insertWidget(1, findAllToolbar);
    findAllToolbar->hide();

    connect(findAllToolbar, &FindToolBar::findAll, m_editor, &CodeEditor::findAll);
    // connect(findAllToolbar, &FindToolBar::findNext, this, &Document::findNext);
    connect(findAllToolbar, &FindToolBar::findPrevious, this, &Document::findPrevious);

    // Emit signals to the mainwindow when redoability/undoability changes
    connect(m_editor, &CodeEditor::undoAvailable, this, &Document::undoAvailable);
    connect(m_editor, &CodeEditor::redoAvailable, this, &Document::redoAvailable);

    connect(m_shareCtrl, &SharedNoteController::highligthedSyntaxChanged, this, &Document::updateHighlighter);

    QList<int> sizeList;
    sizeList << 900 << 100 << 100;
    ui->codeChatSplitter->setSizes(sizeList);
    ui->participantSplitter->setSizes(sizeList);

    startedCollaborating= false;

    updateHighlighter();
    auto text= m_shareCtrl->text();
    if(!text.isEmpty())
        setPlainText(text);
}

Document::~Document()
{
    delete ui;
}
/*
 *
 */

void Document::displayParticipantPanel()
{
    startedCollaborating= true;
    setParticipantsHidden(false);
}

void Document::setEditorFont(QFont font)
{
    m_editor->setFont(font);
    QFontMetrics fm(m_editor->font());
    m_editor->setTabStopDistance(fm.averageCharWidth() * 4);
}

void Document::setParticipantsFont(QFont font)
{
    m_participantPane->setFont(font);
}

void Document::undo()
{
    if(m_editor->hasFocus())
    {
        m_editor->undo();
    }
}

void Document::redo()
{
    if(m_editor->hasFocus())
    {
        m_editor->redo();
    }
}

void Document::cut()
{
    if(m_editor->hasFocus())
    {
        m_editor->cut();
    }
    else if(m_participantPane->hasFocus())
    {
        // do nothing
    }
}

void Document::copy()
{
    if(m_editor->hasFocus())
    {
        m_editor->copy();
    }
    else if(m_participantPane->hasFocus())
    {
        // do nothing
    }
}

void Document::paste()
{
    if(m_editor->hasFocus())
    {
        m_editor->paste();
    }
    else if(m_participantPane->hasFocus())
    {
        // do nothing
    }
}

void Document::setParticipantsHidden(bool b)
{
    if(b)
    {
        ui->participantSplitter->widget(2)->hide();
    }
    else
    {
        ui->participantSplitter->widget(2)->show();
        m_editor->resize(QSize(9000, 9000));
    }
}

void Document::shiftLeft()
{
    m_editor->shiftLeft();
}

void Document::shiftRight()
{
    m_editor->shiftRight();
}

void Document::unCommentSelection()
{
    m_editor->unCommentSelection();
}

/*void Document::fill(NetworkMessageWriter* msg)
{
    msg->string32(m_editor->toPlainText());
    m_participantPane->fill(msg);
}
void Document::readFromMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        if(msg->action() == NetMsg::updateTextAndPermission)
        {
            QString text= msg->string32();
            m_editor->setPlainText(text);
            m_participantPane->readFromMsg(msg);
        }
        else if(msg->action() == NetMsg::updatePermissionOneUser)
        {
            m_participantPane->readPermissionChanged(msg);
        }
    }
}*/

void Document::updateHighlighter()
{
    using SNC= SharedNoteController::HighlightedSyntax;
    switch(m_shareCtrl->highligthedSyntax())
    {
    case SNC::None:
        m_highlighter.reset();
        break;
    case SNC::MarkDown:
        m_highlighter.reset(new MarkDownHighlighter(m_editor->document()));
        break;
    }
}

bool Document::isUndoable()
{
    return m_editor->document()->isUndoAvailable();
}

bool Document::isRedoable()
{
    return m_editor->document()->isRedoAvailable();
}

bool Document::isModified()
{
    return m_editor->document()->isModified();
}

bool Document::isChatHidden()
{
    return ui->codeChatSplitter->widget(1)->isHidden();
}

bool Document::isParticipantsHidden()
{
    return ui->participantSplitter->widget(2)->isHidden();
}

void Document::findAll()
{
    findAllToolbar->show();
    findAllToolbar->giveFocus();
}

void Document::findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if(!m_editor->findNext(searchString, sensitivity, wrapAround, mode))
    {
        emit notFound();
    }
}

void Document::findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if(!m_editor->findPrev(searchString, sensitivity, wrapAround, mode))
    {
        emit notFound();
    }
}

void Document::replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity,
                          Enu::FindMode mode)
{
    if(!m_editor->replaceAll(searchString, replaceString, sensitivity, mode))
    {
        QMessageBox::information(m_editor, tr("Cahoots"), tr("The string was not found."));
    }
}

void Document::replace(QString replaceString)
{
    if(!m_editor->replace(replaceString))
    {
        emit notFound();
    }
}

void Document::findReplace(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity,
                           bool wrapAround, Enu::FindMode mode)
{
    if(!m_editor->findReplace(searchString, replaceString, sensitivity, wrapAround, mode))
    {
        emit notFound();
    }
}
QTextDocument* Document::getDocument()
{
    if(nullptr != m_editor)
        return m_editor->document();
    else
        return nullptr;
}

QString Document::getPlainText()
{
    return m_editor->toPlainText();
}

void Document::setPlainText(QString text)
{
    m_editor->setPlainText(text);
}

void Document::toggleLineWrap()
{
    if(m_editor->lineWrapMode() == QPlainTextEdit::NoWrap)
    {
        m_editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }
    else
    {
        m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void Document::setModified(bool b)
{
    m_editor->document()->setModified(b);
}

void Document::renderMarkdown()
{
    // todo
}

bool Document::docHasCollaborated()
{
    return startedCollaborating;
}

void Document::findNext(QString string)
{
    m_editor->findNext(string, Qt::CaseInsensitive, true, Enu::Contains);
}

void Document::findPrevious(QString string)
{
    m_editor->findPrev(string, Qt::CaseInsensitive, true, Enu::Contains);
}

ParticipantsPane* Document::getParticipantPane() const
{
    return m_participantPane.get();
}

void Document::setParticipantPane(ParticipantsPane* participantPane)
{
    m_participantPane.reset(participantPane);
}
