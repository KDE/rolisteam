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
#ifndef VALUEFNODE_H
#define VALUEFNODE_H

#include <QVariant>

#include "formulanode.h"

namespace Formula
{
    /**
     * @brief The ValueFNode class
     */
    class ValueFNode : public FormulaNode
    {
    public:
        /**
         * @brief ValueFNode
         */
        ValueFNode();
        /**
         * @brief run
         * @param previous
         * @return
         */
        virtual bool run(FormulaNode* previous);
        bool isNumber();
        bool isString();
        void setValue(QVariant);
        virtual QVariant getResult();

        int getPriority();

    private:
        QVariant m_value;
    };
} // namespace Formula
#endif // VALUEFNODE_H
