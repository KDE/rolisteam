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
#include "parsingtoolformula.h"
#include "nodes/operator.h"

#include "nodes/parenthesesfnode.h"
#include "nodes/valuefnode.h"
#include <QDebug>

namespace Formula
{

ParsingToolFormula::ParsingToolFormula()
{
    // ABS,MIN,MAX,IF,FLOOR,CEIL,AVG
    m_hashOp.insert({QStringLiteral("abs"), ABS});
    m_hashOp.insert({QStringLiteral("min"), MIN});
    m_hashOp.insert({QStringLiteral("max"), MAX});
    m_hashOp.insert({QStringLiteral("concat"), CONCAT});
    m_hashOp.insert({QStringLiteral("floor"), FLOOR});
    m_hashOp.insert({QStringLiteral("ceil"), CEIL});
    m_hashOp.insert({QStringLiteral("avg"), AVG});

    m_arithmeticOperation.insert({QStringLiteral("+"), ScalarOperatorFNode::PLUS});
    m_arithmeticOperation.insert({QStringLiteral("-"), ScalarOperatorFNode::MINUS});
    m_arithmeticOperation.insert({QStringLiteral("*"), ScalarOperatorFNode::MULTIPLICATION});
    m_arithmeticOperation.insert({QStringLiteral("x"), ScalarOperatorFNode::MULTIPLICATION});
    m_arithmeticOperation.insert({QStringLiteral("/"), ScalarOperatorFNode::DIVIDE});
    m_arithmeticOperation.insert({QStringLiteral("÷"), ScalarOperatorFNode::DIVIDE});
}
ParsingToolFormula::~ParsingToolFormula() {}

FormulaNode* ParsingToolFormula::getLatestNode(FormulaNode* node)
{
    if(nullptr == node)
        return nullptr;
    FormulaNode* next= node;
    while(nullptr != next->next())
    {
        next= next->next();
    }
    return next;
}

const QHash<QString, QString> ParsingToolFormula::getVariableHash() const
{
    return m_variableHash;
}

void ParsingToolFormula::setVariableHash(const QHash<QString, QString>& variableHash)
{
    m_variableHash= variableHash;
}
bool ParsingToolFormula::readFormula(QString& str, FormulaNode*& previous)
{
    if(str.startsWith('='))
    {
        str= str.remove(0, 1);
    }
    FormulaNode* operandNode= nullptr;
    bool found= false;
    if(readParenthese(str, operandNode))
    {
        previous= operandNode;
        found= true;
    }
    else if(readOperand(str, operandNode))
    {
        previous= operandNode;
        found= true;
    }
    if(found)
    {
        operandNode= getLatestNode(operandNode);
        while(readScalarOperator(str, operandNode))
            ;
    }

    return found;
}
bool ParsingToolFormula::readParenthese(QString& str, FormulaNode*& previous)
{
    if(str.startsWith("("))
    {
        str= str.remove(0, 1);
        FormulaNode* internalNode= nullptr;
        if(readFormula(str, internalNode))
        {
            ParenthesesFNode* node= new ParenthesesFNode();
            node->setInternalNode(internalNode);
            previous= node;

            if(str.startsWith(")"))
            {
                str= str.remove(0, 1);
                return true;
            }
        }
    }
    return false;
}

bool ParsingToolFormula::readScalarOperator(QString& str, FormulaNode* previous)
{
    ScalarOperatorFNode::ArithmeticOperator ope;
    auto it= std::find_if(m_arithmeticOperation.begin(), m_arithmeticOperation.end(),
                          [str](const std::pair<QString, ScalarOperatorFNode::ArithmeticOperator>& pair) {
                              return str.startsWith(pair.first);
                          });

    if(it == m_arithmeticOperation.end())
        return false;

    ope= (*it).second;
    str= str.remove(0, (*it).first.size());

    ScalarOperatorFNode* node= new ScalarOperatorFNode();
    node->setArithmeticOperator(ope);

    FormulaNode* internal= nullptr;
    readFormula(str, internal);

    node->setInternalNode(internal);

    if(nullptr == internal)
    {
        delete node;
        return false;
    }
    if(node->getPriority() >= internal->getPriority())
    {
        node->setNext(internal->next());
        internal->setNext(nullptr);
    }
    previous->setNext(node);
    return true;
}

bool ParsingToolFormula::readOperand(QString& str, FormulaNode*& previous)
{
    if(readNumber(str, previous))
    {
        return true;
    }
    else if(readFieldRef(str, previous))
    {
        return true;
    }
    else if(readOperator(str, previous))
    {
        return true;
    }
    else if(readStringValue(str, previous))
    {
        return true;
    }
    return false;
}
bool ParsingToolFormula::readStringValue(QString& str, FormulaNode*& previous)
{
    if(str.isEmpty())
        return false;

    QString strResult;
    if(str.startsWith("\""))
    {
        int i= 0;
        str= str.remove(0, 1);
        while(i < str.length()
              && str[i] != '"') //&& (str[i].isLetterOrNumber() || str[i].isPunct() || str[i].isSpace())
        {
            strResult+= str[i];
            ++i;
        }

        str= str.remove(0, strResult.size() + 1);
        ValueFNode* nodeV= new ValueFNode();
        nodeV->setValue(strResult);
        previous= nodeV;
        return true;
    }

    return false;
}

bool ParsingToolFormula::readOperator(QString& str, FormulaNode*& previous)
{
    auto it= std::find_if(m_hashOp.begin(), m_hashOp.end(),
                          [str](const std::pair<QString, ParsingToolFormula::FormulaOperator>& pair) {
                              return str.startsWith(pair.first);
                          });

    if(it == m_hashOp.end())
        return false;

    QString key= it->first;
    str= str.remove(0, key.size());
    OperatorFNode* node= new OperatorFNode();
    previous= node;
    node->setOperator(it->second);
    FormulaNode* nextNode= nullptr;
    if(str.startsWith("("))
    {
        str= str.remove(0, 1);
        while(readFormula(str, nextNode)) //&& !str.startsWith(")")
        {                                 // reading parameter loop
            node->addParameter(nextNode);
            nextNode= nullptr;
            if(str.startsWith(","))
            {
                str= str.remove(0, 1);
            }
        }
        if(str.startsWith(")"))
        {
            str= str.remove(0, 1);
        }
    }
    return true;
}

bool ParsingToolFormula::readFieldRef(QString& str, FormulaNode*& previous)
{
    if(str.isEmpty())
        return false;
    if(str.startsWith("${"))
    {
        str= str.remove(0, 2);
    }
    QString key;
    int post= str.indexOf('}');
    key= str.left(post);

    if(m_variableHash.contains(key))
    {
        QString value= m_variableHash.value(key);
        bool ok;
        qreal valueR= value.toDouble(&ok);
        if(ok)
        {
            str= str.remove(0, post + 1);
            ValueFNode* nodeV= new ValueFNode();
            nodeV->setValue(valueR);
            previous= nodeV;
            return true;
        }
    }

    return false;
}

bool ParsingToolFormula::readNumber(QString& str, FormulaNode*& previous)
{
    if(str.isEmpty())
        return false;

    QString number;
    int i= 0;
    while(i < str.length() && ((str[i].isNumber()) || (str[i] == '.') || ((i == 0) && (str[i] == '-'))))
    {
        number+= str[i];
        ++i;
    }

    bool ok;
    qreal r= number.toDouble(&ok);
    if(ok)
    {
        str= str.remove(0, number.size());
        ValueFNode* nodeV= new ValueFNode();
        nodeV->setValue(r);
        previous= nodeV;
        return true;
    }

    return false;
}
} // namespace Formula
