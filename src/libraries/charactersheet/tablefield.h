/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * https://rolisteam.org/                                                *
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
#ifndef TABLEFIELD_H
#define TABLEFIELD_H

#include "charactersheet/charactersheetitem.h"
#include "field.h"
#include <QGraphicsItem>
#include <QLabel>
#include <QObject>
#include <QStandardItemModel>
#include <QWidget>
#ifdef RCSE
#include "tablecanvasfield.h"
#else
class TableCanvasField : public QGraphicsObject
{
    TableCanvasField();
};
#endif

class TableField;
/**
 * @brief The LineFieldItem class
 */
class LineFieldItem : public QObject
{
    Q_OBJECT
public:
    explicit LineFieldItem(QObject* parent= nullptr);
    ~LineFieldItem();
    void insertField(FieldController* field);

    Q_INVOKABLE FieldController* getField(int k) const;

    QList<FieldController*> getFields() const;
    void setFields(const QList<FieldController*>& fields);

    int getFieldCount() const;

    FieldController* getFieldById(const QString& id);
    FieldController* getFieldByLabel(const QString& label);

    void save(QJsonArray& json);
    void load(QJsonArray& json, EditorController* ctrl, CharacterSheetItem* parent);
    void saveDataItem(QJsonArray& json);
    void loadDataItem(QJsonArray& json, CharacterSheetItem* parent);

private:
    QList<FieldController*> m_fields;
};

/**
 * @brief The LineModel class
 */
class LineModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum customRole
    {
        LineRole= Qt::UserRole + 1
    };
    LineModel();
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& data, int role);
    QHash<int, QByteArray> roleNames() const;
    void insertLine(LineFieldItem* line);
    void appendLine(TableField* field);
    void clear();
    int getChildrenCount() const;
    int getColumnCount() const;
    FieldController* getField(int line, int col);
    FieldController* getFieldById(const QString& id);
    void removeLine(int index);
    void save(QJsonArray& json);
    void load(const QJsonArray& json, EditorController* ctrl, CharacterSheetItem* parent);
    void saveDataItem(QJsonArray& json);
    void loadDataItem(const QJsonArray& json, CharacterSheetItem* parent);
    void setChildFieldData(const QJsonObject& json);
    int sumColumn(const QString& name) const;
    void setFieldInDictionnary(QHash<QString, QString>& dict, const QString& id, const QString& label) const;

private:
    QList<LineFieldItem*> m_lines;
};

/**
 * @brief The Field class managed text field in qml and datamodel.
 */
class TableField : public FieldController
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ getModel CONSTANT)

public:
    enum ControlPosition
    {
        CtrlLeftTop,
        CtrlLeftBottom,
        CtrlTopLeft,
        CtrlTopRight,
        CtrlBottomLeft,
        CtrlBottomRight,
        CtrlRightTop,
        CtrlRightBottom
    };
    explicit TableField(bool addCount= true, QGraphicsItem* parent= nullptr);
    explicit TableField(QPointF topleft, bool addCount= true, QGraphicsItem* parent= nullptr);
    void fillModel();
    virtual ~TableField();

    LineModel* getModel() const;

    virtual bool mayHaveChildren() const override;

    virtual void setCanvasField(CanvasField* canvasField) override;

    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const override;

    /// Overriden from charactersheetitem
    virtual bool hasChildren() override;
    virtual int getChildrenCount() const override;
    virtual CharacterSheetItem* getChildFromId(const QString& id) const override;
    virtual CharacterSheetItem* getChildAt(int) const override;
    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json, EditorController* ctrl) override;
    virtual void copyField(CharacterSheetItem* oldItem, bool copyData, bool sameId= true);

    ControlPosition getPosition() const;
    void setPosition(const ControlPosition& position);

    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const override;
    void saveDataItem(QJsonObject& json) override;
    void loadDataItem(const QJsonObject& json) override;
    void setChildFieldData(const QJsonObject& json);

    int getMaxVisibleRowCount() const;

    CharacterSheetItem* getRoot();
    void appendChild(CharacterSheetItem*) override;
    int lineNumber() const;
    int itemPerLine() const;

    Q_INVOKABLE int sumColumn(const QString& name) const;
    void setFieldInDictionnary(QHash<QString, QString>& dict) const override;

public slots:
    void addLine();
    void removeLine(int line);
    void removeLastLine();

signals:
    void lineMustBeAdded(TableField* table);

protected:
    void init();

protected:
    ControlPosition m_position;
    TableCanvasField* m_tableCanvasField= nullptr;
    LineModel* m_model= nullptr;
};

#endif // TABLEFIELD_H
