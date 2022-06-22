/***************************************************************************
 *   Copyright (C) 2016 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef FORMULANODE_H
#define FORMULANODE_H

#include <QVariant>

namespace Formula
{
    /**
     * @brief The FormulaNode class abstract class for all nodes.
     */
    class FormulaNode
    {
    public:
        FormulaNode();
        virtual ~FormulaNode();
        virtual bool run(FormulaNode* previous)= 0;
        FormulaNode* next() const;
        void setNext(FormulaNode* next);

        virtual QVariant getResult();

        virtual int getPriority();

    protected:
        static FormulaNode* getLatestNode(FormulaNode* node);

    protected:
        FormulaNode* m_next;
    };
} // namespace Formula
#endif // FORMULANODE_H
