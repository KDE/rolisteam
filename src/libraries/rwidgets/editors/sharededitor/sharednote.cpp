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
#include "sharednote.h"

#include "ui_sharednote.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextCodec>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include "controller/view_controller/sharednotecontroller.h"
#include "document.h"
#include "enu.h"
#include "utilities.h"

SharedNote::SharedNote(SharedNoteController* ctrl, QWidget* parent)
    : QMainWindow(parent), m_sharedCtrl(ctrl), ui(new Ui::SharedNote)
{
    ui->setupUi(this);

    m_document= new Document(ctrl, ui->m_documentSupport);
    connect(m_document->getDocument(), &QTextDocument::contentsChange, this, &SharedNote::textHasChanged);
    auto pane= m_document->getParticipantPane();

    auto func= [this]() {
        if(!m_sharedCtrl)
            return;
        setWindowTitle(tr("%1 - Shared Note Editor").arg(m_sharedCtrl->name()));
    };

    connect(m_sharedCtrl, &SharedNoteController::nameChanged, this, func);

    connect(pane, &ParticipantsPane::memberCanNowRead, this, &SharedNote::populateDocumentForUser);
    connect(pane, &ParticipantsPane::memberPermissionsChanged, this, &SharedNote::playerPermissionsChanged);
    connect(m_document, &Document::contentChanged, this, [this]() { setWindowModified(true); });

    findDialog= new FindDialog(this);
    connect(findDialog, &FindDialog::findDialogFindNext, this, &SharedNote::findNextTriggered);
    connect(findDialog, &FindDialog::findDialogFindPrev, this, &SharedNote::findPrevTriggered);
    connect(findDialog, &FindDialog::findDialogReplaceAll, this, &SharedNote::replaceAllTriggered);
    connect(findDialog, &FindDialog::findDialogReplace, this, &SharedNote::replaceTriggered);
    connect(findDialog, &FindDialog::findDiaalogFindReplace, this, &SharedNote::findReplaceTriggered);

    QGridLayout* tabLayout= new QGridLayout;
    ui->m_documentSupport->setLayout(tabLayout);
    tabLayout->addWidget(m_document);
    tabLayout->setContentsMargins(0, 0, 0, 0);

    auto localIsOwner= (m_sharedCtrl->ownerId() == m_sharedCtrl->localId());

    connect(m_document, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoability(bool)));
    connect(m_document, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoability(bool)));

    if(localIsOwner)
    {
        connect(ui->m_showParticipants, &QAction::triggered, m_sharedCtrl,
                &SharedNoteController::setParticipantPanelVisible);
        connect(m_sharedCtrl, &SharedNoteController::participantPanelVisibleChanged, ui->m_showParticipants,
                &QAction::setChecked);

        connect(m_sharedCtrl, &SharedNoteController::participantPanelVisibleChanged, pane,
                &ParticipantsPane::setVisible);
        connect(m_sharedCtrl, &SharedNoteController::participantPanelVisibleChanged, this,
                [this](bool b) { m_document->setParticipantsHidden(!b); });

        ui->m_showParticipants->setChecked(m_sharedCtrl->participantPanelVisible());
    }
    else
    {
        ui->m_showParticipants->setChecked(false);
    }

    ui->m_showParticipants->setEnabled(localIsOwner);

    connect(ui->m_highlightMarkdownAction, &QAction::triggered, this, [this](bool b) {
        m_sharedCtrl->setHighligthedSyntax(b ? SharedNoteController::HighlightedSyntax::MarkDown :
                                               SharedNoteController::HighlightedSyntax::None);
    });

    connect(ui->m_markdownPreview, &QAction::triggered, m_sharedCtrl, &SharedNoteController::setMarkdownVisible);

    pane->setVisible(m_sharedCtrl->participantPanelVisible());

    readSettings();

    func();
    qApp->installEventFilter(this);
}

SharedNote::~SharedNote()
{
    delete ui;
}

void SharedNote::readSettings()
{
    QSettings settings("rolisteam", "SharedNote");
    QDesktopWidget* desktop= QApplication::desktop();
    int width= static_cast<int>(desktop->width() * 0.80);
    int height= static_cast<int>(desktop->height() * 0.70);
    int screenWidth= desktop->width();
    int screenHeight= desktop->height();
    QPoint pos= settings.value("pos", QPoint((screenWidth - width) / 2, (screenHeight - height) / 2)).toPoint();
    QSize size= settings.value("size", QSize(width, height)).toSize();
    resize(size);
    move(pos);
}

void SharedNote::writeSettings()
{
    QSettings settings("rolisteam", "SharedNote");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("name", "user");
}

// Protected closeEvent
void SharedNote::closeEvent(QCloseEvent*)
{
    // bool okToQuit = true;
    /// @warning do something
}
void SharedNote::populateDocumentForUser(QString id)
{
    /*NetworkMessageWriter msg(NetMsg::SharedNoteCategory, NetMsg::updateTextAndPermission, NetworkMessage::OneOrMany);
    QStringList ids;
    ids << id;
    msg.setRecipientList(ids, NetworkMessage::OneOrMany);
    msg.string8(m_id);
    m_document->fill(&msg);
    msg.sendToServer();*/
}

void SharedNote::setOwnerId(const QString& id)
{
    updateWindowTitle();
}
bool SharedNote::eventFilter(QObject*, QEvent* event)
{
    if(event->type() == QEvent::FileOpen)
    {
        QString fileName
            = static_cast<QFileOpenEvent*>(event)->file(); // for the sake of mirroring code in our open slot
        if(!fileName.isEmpty())
        {
        }
        event->accept();
        return true;
    }
    return false;
}

bool SharedNote::saveFile(QDataStream& out)
{
    if(nullptr == m_document)
        return false;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << m_document->getPlainText();
    QApplication::restoreOverrideCursor();
    m_document->setModified(false);
    setWindowModified(false);
    return true;
}

bool SharedNote::saveFileAsText(QTextStream& out)
{
    if(nullptr == m_document)
        return false;

    out << m_document->getPlainText();
    m_document->setModified(false);
    setWindowModified(false);
    return true;
}

bool SharedNote::loadFileAsText(QTextStream& in, bool md)
{
    if(nullptr == m_document)
        return false;
    QString data;
    data= in.readAll();
    m_document->setPlainText(data);
    if(md)
        m_sharedCtrl->setHighligthedSyntax(SharedNoteController::HighlightedSyntax::MarkDown);

    return true;
}

bool SharedNote::loadFile(QDataStream& in)
{
    if(nullptr == m_document)
        return false;

    QString data;
    in >> data;
    m_document->setPlainText(data);
    return true;
}

QString SharedNote::strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void SharedNote::on_actionFile_Print_triggered()
{
    QTextDocument document(m_document->getPlainText(), this);
    document.setDefaultFont(QFont("Courier", 10, -1));

    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog* dialog= new QPrintDialog(&printer, this);
    dialog->setWindowTitle("Print");
    if(dialog->exec() == QDialog::Accepted)
    {
        document.print(&printer);
    }
}

// Edit menu items
void SharedNote::on_actionEdit_Undo_triggered()
{
    m_document->undo();
}

void SharedNote::on_actionEdit_Redo_triggered()
{
    m_document->redo();
}

void SharedNote::on_actionEdit_Cut_triggered()
{
    m_document->cut();
}

void SharedNote::on_actionEdit_Copy_triggered()
{
    m_document->copy();
}

void SharedNote::on_actionEdit_Paste_triggered()
{
    m_document->paste();
}

void SharedNote::on_actionEdit_Find_All_triggered()
{
    m_document->findAll();
}

void SharedNote::on_actionEdit_Find_triggered()
{
    findDialog->show();
}

void SharedNote::on_actionView_Line_Wrap_triggered()
{
    m_document->toggleLineWrap();
}

//#include "userlist/playersList.h"
void SharedNote::textHasChanged(int pos, int charsRemoved, int charsAdded)
{
    if(!m_sharedCtrl->localCanWrite())
        return;

    QString toSend;
    QString data;

    if(charsRemoved > 0 && charsAdded == 0)
    {
        data= "";
    }
    else if(charsAdded > 0)
    {
        QTextCursor cursor= QTextCursor(m_document->getDocument());
        cursor.setPosition(pos, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        data= cursor.selection().toPlainText();
    }

    toSend= QString("doc:%1 %2 %3 %4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data);

    m_sharedCtrl->setTextUpdate(toSend);
}

void SharedNote::displaySharingPanel()
{
    m_document->displayParticipantPanel();
}

void SharedNote::updateDocumentToAll(NetworkMessageWriter* msg)
{
    /* if(nullptr != m_document)
     {
         m_document->fill(msg);
     }*/
}

void SharedNote::on_actionText_Shift_Left_triggered()
{
    m_document->shiftLeft();
}

void SharedNote::on_actionText_Shift_Right_triggered()
{
    m_document->shiftRight();
}

void SharedNote::on_actionText_Comment_Line_triggered()
{
    m_document->unCommentSelection();
}

void SharedNote::setUndoability(bool b)
{
    if(sender() == m_document)
    {
        ui->actionEdit_Undo->setEnabled(b);
    }
}

void SharedNote::setRedoability(bool b)
{
    if(sender() == m_document)
    {
        ui->actionEdit_Redo->setEnabled(b);
    }
}

void SharedNote::documentChanged()
{
    Document* document= m_document;
    ui->actionEdit_Undo->setEnabled(document->isUndoable());
    ui->actionEdit_Redo->setEnabled(document->isRedoable());
    // ui->actionWindow_Split->setDisabled(document->isEditorSplit() && !document->isEditorSplitSideBySide());
    // ui->actionWindow_Split_Side_by_Side->setDisabled(document->isEditorSplit() &&
    // document->isEditorSplitSideBySide());

    // ui->actionWindow_Remove_Split->setEnabled(document->isEditorSplit());
    ui->actionTools_Announce_Document->setDisabled(document->docHasCollaborated());
}

void SharedNote::tabCloseClicked()
{
    /*   if (maybeSave(index)) {
           if (ui->tabWidget->count() == 1) {
               on_actionFile_New_triggered();
           }
           ui->tabWidget->widget(index)->deleteLater();
           tabWidgetToDocumentMap.remove(ui->tabWidget->widget(index));
           ui->tabWidget->removeTab(index);
       }*/
}

void SharedNote::findNextTriggered(QString str, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    m_document->findNext(str, sensitivity, wrapAround, mode);
}

void SharedNote::findPrevTriggered(QString str, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    m_document->findPrev(str, sensitivity, wrapAround, mode);
}

void SharedNote::replaceAllTriggered(QString find, QString replace, Qt::CaseSensitivity sensitivity, Enu::FindMode mode)
{
    m_document->replaceAll(find, replace, sensitivity, mode);
}

void SharedNote::replaceTriggered(QString replace)
{
    m_document->replace(replace);
}

void SharedNote::findReplaceTriggered(QString find, QString replace, Qt::CaseSensitivity sensitivity, bool wrapAround,
                                      Enu::FindMode mode)
{
    m_document->findReplace(find, replace, sensitivity, wrapAround, mode);
}

void SharedNote::playerPermissionsChanged(QString id, int perm)
{
    // QString toSend = QString("updateperm:%1").arg(permissions);
    /*NetworkMessageWriter msg(NetMsg::SharedNoteCategory, NetMsg::updatePermissionOneUser);
    msg.string8(m_id); // MediaId
    msg.string8(id);   // playerId
    msg.int8(perm);    // Permission
    msg.sendToServer();*/
}

void SharedNote::setEditorFont(QFont font)
{
    m_document->setEditorFont(font);
}

void SharedNote::setParticipantsFont(QFont font)
{
    m_document->setParticipantsFont(font);
}

QString SharedNote::id() const
{
    return m_id;
}

void SharedNote::setId(const QString& id)
{
    m_id= id;
}

void SharedNote::updateWindowTitle()
{

    /*PlayerModel* list= PlayerModel::instance();
    Player* player= list->getLocalPlayer();
    setWindowTitle(
        tr("%1[*] - SharedNote - %2").arg(m_fileName, m_document->canWrite(player) ? tr("ReadWrite") :
    tr("ReadOnly")));*/
}

void SharedNote::on_m_markdownPreview_triggered()
{
    if(nullptr == m_document)
        return;

    m_document->renderMarkdown();
}
