/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CHARACTERSHEETMODEL_H
#define CHARACTERSHEETMODEL_H

#include <QAbstractItemModel>

#include <QFile>
#include <QPointF>
#include <QTextStream>

#include <charactersheet/charactersheet_global.h>

#include "charactersheetitem.h"

class CharacterSheet;
class Section;

namespace Formula
{
class FormulaManager;
}
/**
 * @brief CharacterSheetModel is part of the MVC architecture for charactersheet viewer. it herits from
 * QAbstractItemModel it also provides features for adding data into stored CharacterSheet.
 */
class CHARACTERSHEET_EXPORT CharacterSheetModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum CustomRole
    {
        FormulaRole= Qt::UserRole + 1,
        ValueRole,
        UuidRole,
        NameRole
    };
    Q_ENUM(CustomRole)
    /**
     * @brief default constructor
     */
    CharacterSheetModel();
    virtual ~CharacterSheetModel();

    /**
     * @brief compulsory function which returns the  row count : should be the higher index count of stored
     * characterSheets
     * @param parent useless in tablemodel (only relevant with a tree.)
     */
    int rowCount(const QModelIndex& parent= QModelIndex()) const;
    /**
     * @brief build an index with the given location and parent.
     * It should return invalide index when location is out of data range.
     * @param row : the row coordinate
     * @param column : the column coordinate
     * @param parent : all items in the table should be child of an invalide QModelIndex.
     */
    QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const;
    /**
     * @brief build the parent of given index. Useless in our case.
     */
    QModelIndex parent(const QModelIndex& index) const;
    /**
     * @brief compulsory function which returns the column count: it is egal to the number of characterSheet stored in
     * the model.
     * @param : parent useless because in a tableview/model all items have the same parent (root).
     */
    int columnCount(const QModelIndex& parent= QModelIndex()) const;
    /**
     * @brief compulsory function which returns the value of the given index.
     * @param index : the location of the wished element
     * @param role : the data role.
     */
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const;
    /**
     * @brief allows editing. The model can modify the data beacause of the function.
     * @param index : location of the amended data.
     * @param value : new value
     * @param role : the data role
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole);

    /**
     * @brief adds section after the given Index.
     * @param index location of the new section
     */
    void addSection(/*int index*/);
    /**
     * @brief adds line at the given index
     * @param index location of the new line.
     */
    void addLine(const QModelIndex& index);

    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool hasChildren(const QModelIndex& parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    CharacterSheetItem* indexToSection(const QModelIndex& index);
    QModelIndex indexToSectionIndex(const QModelIndex& index);

    CharacterSheet* getCharacterSheet(int id);

    // QList<CharacterSheetItem *>* getExportedList(CharacterSheet*);

    bool writeModel(QJsonObject& file, bool data= true);
    void readModel(const QJsonObject& file, bool readRootSection);
    void setRootSection(const QJsonObject& file);
    QJsonObject rootSectionData() const;

    CharacterSheetItem* addSection(QString title);
    void addLine(CharacterSheetItem* parentItem, QString name, const QModelIndex& parent);

    void setRootSection(Section* rootSection);

    Section* getRootSection() const;

    void addCharacterSheet(CharacterSheet* sheet, int pos);

    CharacterSheet* getCharacterSheetById(QString id);

    int getCharacterSheetCount() const;

    void removeCharacterSheet(int index);
    void removeCharacterSheet(CharacterSheet* sheet);
    /**
     * @brief adds an empty CharacterSheet into the model.
     */
    CharacterSheet* addCharacterSheet();
public slots:
    void clearModel();

    void checkCharacter(Section* section);
    void addSubChildRoot(CharacterSheetItem* item);
    void fieldHasBeenChanged(CharacterSheet* sheet, CharacterSheetItem* item, const QString&);
    void addSubChild(CharacterSheet* sheet, CharacterSheetItem* item);

signals:
    void characterSheetHasBeenAdded(CharacterSheet* sheet);
    void dataCharacterChange();

protected:
    void computeFormula(QString path, CharacterSheet* sheet);

private:
    void checkTableItem();

private:
    /**
     * @brief QList which stores pointer to CharacterSheet.
     */
    QList<CharacterSheet*>* m_characterList= nullptr;
    Section* m_rootSection= nullptr;
    Formula::FormulaManager* m_formulaManager= nullptr;
};

#endif // CHARACTERSHEETMODEL_H
