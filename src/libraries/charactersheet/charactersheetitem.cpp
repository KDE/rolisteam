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
#include "charactersheet/charactersheetitem.h"
#include <QDebug>
#include <QTextDocument>
//////////////////////////////
// Item
/////////////////////////////
CharacterSheetItem::CharacterSheetItem()
    : m_parent(nullptr), m_orig(nullptr), m_page(0), m_readOnly(false), m_hasDefaultValue(false)
{
}

bool CharacterSheetItem::hasChildren()
{
    return false;
}

int CharacterSheetItem::getChildrenCount() const
{
    return 0;
}

QVariant CharacterSheetItem::getValueFrom(CharacterSheetItem::ColumnId i, int role) const
{
    Q_UNUSED(i);
    Q_UNUSED(role);
    return QVariant();
}

bool CharacterSheetItem::isReadOnly() const
{
    return m_readOnly;
}

void CharacterSheetItem::setReadOnly(bool readOnly)
{
    if(m_readOnly != readOnly)
    {
        m_readOnly= readOnly;
        emit readOnlyChanged();
        emit characterSheetItemChanged(this);
    }
}

int CharacterSheetItem::getPage() const
{
    return m_page;
}

void CharacterSheetItem::setPage(int page)
{
    if(page != m_page)
    {
        m_page= page;
        emit pageChanged();
    }
}

QString CharacterSheetItem::getFormula() const
{
    return m_formula;
}

void CharacterSheetItem::setFormula(const QString& formula)
{
    m_formula= formula;
}

CharacterSheetItem* CharacterSheetItem::getOrig() const
{
    return m_orig;
}

void CharacterSheetItem::setOrig(CharacterSheetItem* orig)
{
    m_orig= orig;
    if(nullptr != m_orig)
    {
        connect(m_orig, SIGNAL(labelChanged()), this, SLOT(updateLabelFromOrigin()));
    }
}
void CharacterSheetItem::updateLabelFromOrigin()
{
    if(nullptr != m_orig)
    {
        QString oldKey= m_label;

        setLabel(m_orig->getLabel());

        if(nullptr != m_parent)
        {
            m_parent->changeKeyChild(oldKey, m_label, this);
        }
    }
}

void CharacterSheetItem::updateNeeded() {}

QString CharacterSheetItem::getTooltip() const
{
    return m_tooltip;
}

void CharacterSheetItem::setTooltip(const QString& tooltip)
{
    m_tooltip= tooltip;
}
void CharacterSheetItem::changeKeyChild(QString oldkey, QString newKey, CharacterSheetItem* child)
{
    Q_UNUSED(oldkey);
    Q_UNUSED(newKey);
    Q_UNUSED(child);
}

QString CharacterSheetItem::value() const
{
    return m_value;
}

void CharacterSheetItem::setValue(const QString& value, bool fromNetwork)
{
    if(m_readOnly && !fromNetwork)
        return;

    /// @warning ugly solution to prevent html rich text to break the change check.
    m_hasDefaultValue= false;
    QString newValue;
    QString currentValue;
    if(m_currentType <= SELECT)
    {
        QTextDocument doc;
        doc.setHtml(value);
        newValue= doc.toPlainText();

        doc.setHtml(m_value);
        currentValue= doc.toPlainText();
    }
    else
    {
        newValue= value;
        currentValue= m_value;
    }

    if(currentValue != newValue)
    {
        m_value= value;
        emit valueChanged();
        if(!fromNetwork)
        {
            emit characterSheetItemChanged(this);
        }
    }
}

QString CharacterSheetItem::getId() const
{
    return m_id;
}

QString CharacterSheetItem::getLabel() const
{
    return m_label;
}

void CharacterSheetItem::setLabel(const QString& label)
{
    if(m_label != label)
    {
        m_label= label;
        emit labelChanged();
    }
}
void CharacterSheetItem::setId(const QString& id)
{
    m_id= id;
}

bool CharacterSheetItem::removeChild(CharacterSheetItem*)
{
    return false;
}

bool CharacterSheetItem::deleteChild(CharacterSheetItem*)
{
    return false;
}
int CharacterSheetItem::rowInParent()
{
    if(nullptr == m_parent)
        return -1;
    return m_parent->indexOfChild(this);
}
bool CharacterSheetItem::mayHaveChildren() const
{
    return false;
}
CharacterSheetItem* CharacterSheetItem::getChildAt(int) const
{
    return nullptr;
}
QString CharacterSheetItem::getPath()
{
    QString path;
    if(nullptr != m_parent)
    {
        path= m_parent->getPath();
        if(!path.isEmpty())
        {
            path.append('.');
        }
    }
    return path.append(m_id);
}
void CharacterSheetItem::appendChild(CharacterSheetItem*) {}
CharacterSheetItem* CharacterSheetItem::getParent() const
{
    return m_parent;
}

void CharacterSheetItem::setParent(CharacterSheetItem* parent)
{
    m_parent= parent;
}
int CharacterSheetItem::indexOfChild(CharacterSheetItem* itm)
{
    Q_UNUSED(itm);
    return 0;
}
bool CharacterSheetItem::hasFormula() const
{
    return !m_formula.isEmpty();
}
CharacterSheetItem::TypeField CharacterSheetItem::getFieldType() const
{
    return m_currentType;
}

void CharacterSheetItem::setCurrentType(const CharacterSheetItem::TypeField& currentType)
{
    m_currentType= currentType;
    if(m_currentType == CharacterSheetItem::FUNCBUTTON && m_hasDefaultValue)
    {
        m_value= "";
    }
}

void CharacterSheetItem::setFieldInDictionnary(QHash<QString, QString>& dict) const
{
    auto val= value();
    dict[m_id]= val;
    dict[getLabel()]= val;
}

void CharacterSheetItem::initGraphicsItem() {}
