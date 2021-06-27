/**
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
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>

#include "codeeditor.h"
#include "enu.h"
#include "findtoolbar.h"
#include "network/networkmessagewriter.h"
#include "participantmodel.h"
#include "participantspane.h"
#include <QSyntaxHighlighter>

//#include "client.h"
//#include "server.h"

namespace Ui
{
class Document;
}
class SharedNoteController;
class Document : public QWidget
{
    Q_OBJECT
public:
    Document(SharedNoteController* ctrl, QWidget* parent= nullptr);
    ~Document();

    // This sets up the document so people can connect to it.
    // Hopefully we can do something with Bonjour so you can browse for local documents
    // but that's down the road.
    void displayParticipantPanel();
    void setEditorFont(QFont font);
    void setParticipantsFont(QFont font);

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();

    // shows/hides the participants pane
    void setParticipantsHidden(bool b);
    // shifts the current line/selected lines left
    void shiftLeft();
    // shifts the current line/selected lines right
    void shiftRight();
    // C++ style (un)commenting
    void unCommentSelection();

    // User wants to resynchronize the document with the owner
    // void fill(NetworkMessageWriter* msg);
    // void readFromMsg(NetworkMessageReader* msg);

    // returns if the editor is undable
    bool isUndoable();
    // returns if the editor is redoable
    bool isRedoable();
    // returns if the editor is modified (unsaved since last edit)
    bool isModified();
    // returns if the chat pane is hidden, used for determining which actions to enable/disable by the main window
    bool isChatHidden();
    // returns if the participants pane is hidden, used for determining which actions to enable/disable in the MW
    bool isParticipantsHidden();

    // Find functions
    void findAll();
    void findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);
    void findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);
    void replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, Enu::FindMode mode);
    void replace(QString replaceString);
    void findReplace(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround,
                     Enu::FindMode mode);

    QString getPlainText();
    void setPlainText(QString text);

    QTextDocument* getDocument();

    void toggleLineWrap();
    void setModified(bool b);
    void renderMarkdown();
    bool docHasCollaborated();

    void setOwnerName(QString name);

    ParticipantsPane* getParticipantPane() const;
    void setParticipantPane(ParticipantsPane* participantPane);

signals:
    void redoAvailable(bool);
    void undoAvailable(bool);
    void notFound();
    void contentChanged();

private slots:
    // triggered by the find toolbar, not the dialog
    void findNext(QString string);
    void findPrevious(QString string);
    void updateHighlighter();

private:
    QPointer<SharedNoteController> m_shareCtrl;
    Ui::Document* ui= nullptr;
    CodeEditor* m_editor= nullptr;
    QTextEdit* m_previewMarkdown= nullptr;

    bool startedCollaborating;
    FindToolBar* findAllToolbar= nullptr;
    std::unique_ptr<ParticipantsPane> m_participantPane;
    std::unique_ptr<QSyntaxHighlighter> m_highlighter;
};

#endif // DOCUMENT_H
