/*************************************************************************
 *     Copyright (C) 2010 by Joseph Boudou                               *
 *     Copyright (C) 2011 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QStyledItemDelegate>


#include "preferences/dicealiasmodel.h"
#include "preferences/preferencesmanager.h"
#include "widgets/colorbutton.h"

#include "diceparser/diceparser.h"

#include "widgets/filedirchooser.h"

class CheckBoxDelegate : public QStyledItemDelegate
{
public:
    virtual QWidget*	createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual void	setEditorData(QWidget * editor, const QModelIndex & index) const;
    virtual void	setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};

namespace Ui {
class PreferencesDialogBox;
}

/**
 * @brief Actually only to change directories.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief PreferencesDialog
     * @param parent
     * @param f
     */
    PreferencesDialog(QWidget * parent = NULL, Qt::WindowFlags f = 0);
    /**
     * @brief ~PreferencesDialog
     */
    virtual ~PreferencesDialog();

    /**
     * @brief sendOffAllDiceAlias
     */
    void sendOffAllDiceAlias(NetworkLink*);

public slots:
    /**
     * @brief show
     */
    void show();

private slots:
    /**
     * @brief load
     */
    void load();
    /**
     * @brief save
     */
    void save() const;

    /**
     * @brief performDiag start diagnostic and Display some value about current qt version.
     */
    void performDiag();
	//Management of DiceAliases
    /**
     * @brief managedAction
     */
    void managedAction();
    /**
     * @brief testAliasCommand
     */
    void testAliasCommand();
    /**
     * @brief applyBackground
     */
    void applyBackground();

private:
    PreferencesManager* m_preferences;
    Ui::PreferencesDialogBox* ui;
	DiceParser* m_diceParser;
	DiceAliasModel* m_aliasModel;
    QPushButton* m_applyBtn;
};

#endif
