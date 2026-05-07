/***************************************************************************
 *   Copyright (C) 2026 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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

#include "helper.h"
#include <QAbstractItemModelTester>
#include <QModelIndex>
#include <QModelIndexList>
#include <QPointer>
#include <QTest>
#include <QtCore/QString>
#include <memory>

#include "rwidgets/gmtoolbox/NameGenerator/namegeneratorwidget.h"

class WidgetTester : public NameGeneratorWidget
{
public:
    WidgetTester() {};

    void callbuildAllNames(int count, QHash<QString, DataBase> data) { buildAllNames(count, data); };
    void callCheckFeatureAvailability() { checkFeatureAvailability(); };
    void callGenerateName() { generateName(); };
    bool callNextCharacterCanEnd(const QJsonObject& json, QString key) { return nextCharacterCanEnd(json, key); };
    bool callNextIsPossible(const QJsonObject& json, QString key, bool last)
    {
        return nextIsPossible(json, key, last);
    };
    QString callBuildName(const QJsonObject& json) { return buildName(json); };
    QString callPickUpName(QStringList data) { return pickUpName(data); };

    const QHash<TypeOfGeneration, QHash<QString, DataBase>>& getData() const { return m_complexName; }
};

// using namespace GMTOOL;

class NameGenerator : public QObject
{
    Q_OBJECT
public:
    NameGenerator();

private slots:
    void nameGeneratorTest();

private:
    std::unique_ptr<WidgetTester> m_widget;
};

NameGenerator::NameGenerator() {}

void NameGenerator::nameGeneratorTest()
{
    m_widget.reset(new WidgetTester);
    auto& hash= m_widget->getData();
    for(auto [key, value] : hash.asKeyValueRange())
    {

        QHash<QString, DataBase> data;

        const QHash<QString, DataBase>& data2= value;
        QJsonObject empty;
        QJsonObject definedJson;

        QStringList emptyList;
        QStringList fullList{Helper::randomString(), Helper::randomString(), Helper::randomString(),
                             Helper::randomString(), Helper::randomString(), Helper::randomString(),
                             Helper::randomString()};

        m_widget->callbuildAllNames(2, data);
        m_widget->callbuildAllNames(2, data2);
        m_widget->callCheckFeatureAvailability();
        m_widget->callGenerateName();
        // empty json
        m_widget->callNextCharacterCanEnd(empty, Helper::randomString());
        m_widget->callNextIsPossible(empty, Helper::randomString(), true);
        m_widget->callNextIsPossible(empty, Helper::randomString(), false);
        m_widget->callBuildName(empty);

        // defined json
        m_widget->callNextCharacterCanEnd(definedJson, Helper::randomString());
        m_widget->callNextIsPossible(definedJson, Helper::randomString(), true);
        m_widget->callNextIsPossible(definedJson, Helper::randomString(), false);
        m_widget->callBuildName(definedJson);

        // list
        QVERIFY(m_widget->callPickUpName(emptyList).isEmpty());
        auto res= m_widget->callPickUpName(fullList);
        QVERIFY(fullList.contains(res));
    }
}

QTEST_MAIN(NameGenerator);

#include "tst_namegenerator.moc"
