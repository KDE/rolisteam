/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#ifndef CHARACTERSHEET_H
#define CHARACTERSHEET_H
#include <QString>
#include <QMap>
#include <QVariant>
//typedef QMap<QString,QString> Section;
/**
    * @brief Section stores any fields of specific section
    */
class  Section : public QList<QString>
{
    
public:
    /**
    * Constructor
    */
    Section();
    
    
    /**
    * @brief allows to get the section's name.
    * @return name QString
    */
    QString getName();
    
    /**
    * @brief allows to set the name of this section.
    * @param QString name
    */
    void setName(QString name);
    
    /**
    * @brief allows to get the section's value.
    * @return value QString
    */
    QString getValue();
    
    /**
    * @brief allows to set the value of this section.
    * @param QString value
    */
    void setValue(QString& name);
    
    
private:
    /**
    * @brief stores the section name
    */
    QString m_name;
    /**
    * @brief can be used as default value for all subsections (Star Wars D6, some rolemaster editions and others...)
    */
    QString m_value;
    
    
};
/**
    * @brief the characterSheet stores Section as many as necessary
    */
class CharacterSheet
{
public:
    
    /**
    * Constructor
    */
    CharacterSheet();
    
    /**
    * @brief allows to get the data stored in the given section and associated with the given key.
    * @param section Id of section
    * @param key the id key.
    */
    const QString getData(int section,int key);
    /**
    * @brief allows to get the title(name) of the given section
    * @param section id
    */
    const QString getTitleSection(int section);
    /**
    * @brief allows to get the value of the given section
    * @param section id
    */
    const QString getSectionValue(int section);
    void setSectionValue(int section,QString& value);
    /**
    * @brief global getter of data.  This function has been written to make easier the MVC architecture.
    * @param int index : 0 refers to the title of the first section, 1 refers to the first data of the first section....
    */
    const  QString getData(int index);
    /**
    * @brief allows to get the key, this function is used for displaying the meaning of fields
    * @param int index : 0 refers to the title of the section, 1 refers to key of the first data of the first section...
    */
    const QString getkey(int index);
    
    /**
    * @brief translate a index into a section index
    * @param int index : 0 refers to the first section, if the first section stored 4 fields, then 5 refers to the second section.
    */
    int getSection(int index);
    
    /**
    * @brief allows to register data,
    * @param int index : as getData
    * @param value the value of the added data.
    * @param isHeader true when add a section.
    */
    void setData(int indexSec,int index,QVariant value,bool isHeader = false);
    void addData(int indexSec,int index,QVariant value);
    /**
    * @brief return the number of fields: Sum(number of section + sum of all sections items.)
    */
    int getIndexCount();
    
    /**
    *@brief create an empty section with one row. useful for editing.
    */
    void appendSection(Section* sec);
    
    bool removeSection(Section* sec);
    void removeSectionAt(int index);
    
    void appendLine(int sectionIndex);
    
    int getSectionCount();
    
    void setOwner(QString owner);
    const QString& owner() const;
    
private:
    /**
    * @brief stores all character sheet sections
    */
    QList<Section> m_sectionList;
    /**
    *@brief User Id of the owner
    */
    QString m_owner;
};

#endif // CHARACTERSHEET_H
