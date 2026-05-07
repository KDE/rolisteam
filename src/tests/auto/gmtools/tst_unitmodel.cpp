/***************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                                *
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

#include "rwidgets/gmtoolbox/UnitConvertor/convertor.h"
#include "rwidgets/gmtoolbox/UnitConvertor/convertoroperator.h"
#include "rwidgets/gmtoolbox/UnitConvertor/customrulemodel.h"
#include "rwidgets/gmtoolbox/UnitConvertor/unit.h"
#include "rwidgets/gmtoolbox/UnitConvertor/unitmodel.h"

using namespace GMTOOL;
class TestUnitModel : public QObject
{
    Q_OBJECT
public:
    TestUnitModel();

private slots:
    void init();

    void addTest();
    void addTest_data();

    void addCatTest();
    void addCatTest_data();

    void convertorOpTest();
    void convertorOpTest_data();
    void unitTest();

    void convertorTest();
    void customRulesTest();

private:
    std::unique_ptr<UnitModel> m_model;
    std::unique_ptr<CategoryModel> m_category;
    std::unique_ptr<CustomRuleModel> m_rules;

    QHash<QPair<const Unit*, const Unit*>, ConvertorOperator*> m_convertorTable;
};

TestUnitModel::TestUnitModel() {}

void TestUnitModel::init()
{
    m_model.reset(new UnitModel());
    new QAbstractItemModelTester(m_model.get(), this);

    m_category.reset(new CategoryModel);
    m_category->setSourceModel(m_model.get());
    // new QAbstractItemModelTester(m_category.get(), this);

    m_rules.reset(new CustomRuleModel);
    // new QAbstractItemModelTester(m_rules.get(), this);
}

void TestUnitModel::addTest()
{
    QFETCH(QStringList, nameList);
    QFETCH(QStringList, symbolList);
    QFETCH(QList<int>, category);
    QFETCH(int, expected);

    QVERIFY(m_model->rowCount() == 0);

    QList<Unit*> units;
    int i= 0;
    for(const auto& name : nameList)
    {
        auto symbol= symbolList[i];
        auto cat= category[i];
        auto unit= new Unit(name, symbol, static_cast<Unit::Category>(cat));
        m_model->insertData(unit);
        QVERIFY(m_model->getIndex(unit) >= 0);
        ++i;
    }

    m_model->data(QModelIndex(), Qt::DisplayRole);
    m_model->data(m_model->index(expected + 100, 0), Qt::DisplayRole);
    m_model->headerData(0, Qt::Vertical, Qt::DisplayRole);
    m_model->getUnitByIndex(-100);

    m_model->getUnitByIndex(expected + 100);

    QCOMPARE(m_model->rowCount(), expected);

    for(auto u : units)
    {
        m_model->removeUnit(u);
        m_model->getIndex(u);
    }
    i= 0;
    for(const auto& name : nameList)
    {
        Q_UNUSED(name)
        auto symbol= symbolList[i];
        auto cat= category[i];
        // auto unit= new Unit(name, symbol, static_cast<Unit::Category>(cat));

        m_model->insertUnit(static_cast<Unit::Category>(cat));

        // QVERIFY(m_model->getIndex(unit) >= 0);
        ++i;
    }

    m_model->insertUnit(static_cast<Unit::Category>(100000));
}

void TestUnitModel::addTest_data()
{
    QTest::addColumn<QStringList>("nameList");
    QTest::addColumn<QStringList>("symbolList");
    QTest::addColumn<QList<int>>("category");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QStringList() << QStringList() << QList<int>() << 0;
    QTest::addRow("list2") << QStringList({"Kilometer"}) << QStringList({"km"}) << QList<int>({Unit::DISTANCE}) << 1;
    QTest::addRow("list3") << QStringList({"Kilometer", "meter"}) << QStringList({"km", "m"})
                           << QList<int>({Unit::DISTANCE, Unit::DISTANCE}) << 2;
    QTest::addRow("list4") << QStringList({"Kilometer", "meter", "gram"}) << QStringList({"km", "m", "g"})
                           << QList<int>({Unit::DISTANCE, Unit::DISTANCE, Unit::MASS}) << 3;
}

void TestUnitModel::addCatTest()
{
    QFETCH(QStringList, nameList);
    QFETCH(QStringList, symbolList);
    QFETCH(QList<int>, category);
    QFETCH(int, expected);

    QVERIFY(m_category->rowCount() == 0);

    int i= 0;
    for(const auto& name : nameList)
    {
        auto symbol= symbolList[i];
        auto cat= category[i];
        m_category->addUnit(new Unit(name, symbol, static_cast<Unit::Category>(cat)));
        m_category->currentCategory();
    }

    m_category->data(m_category->index(100, 0, QModelIndex()));
    // QCOMPARE(m_category->rowCount(), expected);
}
void TestUnitModel::addCatTest_data()
{
    QTest::addColumn<QStringList>("nameList");
    QTest::addColumn<QStringList>("symbolList");
    QTest::addColumn<QList<int>>("category");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QStringList() << QStringList() << QList<int>() << 0;
    QTest::addRow("list2") << QStringList({"Kilometer"}) << QStringList({"km"}) << QList<int>({Unit::DISTANCE}) << 1;
    QTest::addRow("list3") << QStringList({"Kilometer", "meter"}) << QStringList({"km", "m"})
                           << QList<int>({Unit::DISTANCE, Unit::DISTANCE}) << 2;
    QTest::addRow("list4") << QStringList({"Kilometer", "meter", "gram"}) << QStringList({"km", "m", "g"})
                           << QList<int>({Unit::DISTANCE, Unit::DISTANCE, Unit::MASS}) << 3;
}

void TestUnitModel::convertorOpTest()
{
    QFETCH(double, a);
    QFETCH(double, b);
    QFETCH(bool, fraction);
    QFETCH(bool, readOnly);
    QFETCH(qreal, origin);
    QFETCH(qreal, dest);

    ConvertorOperator ops(a, b, fraction, readOnly);

    QCOMPARE(ops.convert(origin), dest);

    if(ops.isReadOnly())
    {
        ops.setA(Helper::generate<double>(a + 1, a + 10000));
        QCOMPARE(ops.a(), a);
        ops.setB(Helper::generate<double>(b + 1, a + 10000));
        QCOMPARE(ops.b(), b);
        ops.setFraction(!ops.fraction());
        QCOMPARE(ops.fraction(), fraction);
    }
    else
    {
        ops.setA(Helper::generate<double>(a + 1, a + 10000));
        QCOMPARE_NE(ops.a(), a);
        ops.setB(Helper::generate<double>(b + 1, a + 10000));
        QCOMPARE_NE(ops.b(), b);
        ops.setFraction(!ops.fraction());
        QCOMPARE_NE(ops.fraction(), fraction);
    }
}

void TestUnitModel::convertorOpTest_data()
{
    QTest::addColumn<double>("a");
    QTest::addColumn<double>("b");
    QTest::addColumn<bool>("fraction");
    QTest::addColumn<bool>("readOnly");
    QTest::addColumn<qreal>("origin");
    QTest::addColumn<qreal>("dest");

    QTest::addRow("cmd1") << 1. << 0. << false << true << 100. << 100.;
    QTest::addRow("cmd2") << 1. << 0. << false << false << 100. << 100.;
    QTest::addRow("cmd3") << 1. << 0. << true << false << 100. << 100.;
    QTest::addRow("cmd4") << 1. << 0. << true << true << 100. << 100.;

    QTest::addRow("cmd11") << 10. << 0. << false << false << 10. << 100.;
    QTest::addRow("cmd12") << 10. << 0. << false << true << 10. << 100.;
    QTest::addRow("cmd13") << 10. << 0. << true << false << 10. << 1.;
    QTest::addRow("cmd14") << 10. << 0. << true << true << 10. << 1.;

    QTest::addRow("cmd122") << 10. << 10. << false << false << 10. << 110.;
    QTest::addRow("cmd123") << 10. << 10. << false << true << 10. << 110.;

    QTest::addRow("cmd124") << 10. << 10. << true << false << 10. << 2.;
    QTest::addRow("cmd125") << 10. << 10. << true << true << 10. << 2.;
}

void TestUnitModel::unitTest()
{
    Unit unit;

    auto cat= static_cast<Unit::Category>(Helper::generate<int>(Unit::TEMPERATURE, Unit::CUSTOM));

    unit.setCurrentCat(cat);
    QCOMPARE(unit.currentCat(), cat);

    unit.setReadOnly(true);
    unit.setReadOnly(false);

    auto name= Helper::randomString();
    unit.setName(name);
    QCOMPARE(unit.name(), name);

    auto symbole= Helper::randomString();
    unit.setSymbol(symbole);
    QCOMPARE(unit.symbol(), symbole);

    Unit unit2(unit);
    Unit unit3;
    unit3= unit2;

    // QVERIFY(unit2 == unit)

    QCOMPARE(unit2.currentCat(), cat);
    QCOMPARE(unit2.readOnly(), false);
    QCOMPARE(unit2.name(), name);
    QCOMPARE(unit2.symbol(), symbole);
}

void TestUnitModel::convertorTest()
{
    Convertor con;
    // con.readSettings();
    con.setModified(true);
    // con.writeSettings();
}

void TestUnitModel::customRulesTest()
{

    Unit* gram= m_model->insertData(new Unit(QStringLiteral("gram"), QStringLiteral("g"), Unit::MASS));
    Unit* kilo= m_model->insertData(new Unit(QStringLiteral("kilogram"), QStringLiteral("Kg"), Unit::MASS));
    Unit* once= m_model->insertData(new Unit(QStringLiteral("Once"), QStringLiteral("oz"), Unit::MASS));
    Unit* pound= m_model->insertData(new Unit(QStringLiteral("Pound"), QStringLiteral("lb"), Unit::MASS));
    pound->setReadOnly(true);

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(gram, kilo), new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(gram, once), new ConvertorOperator(1.0 / 28.349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(gram, pound), new ConvertorOperator(1.0 / 453.59237));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kilo, gram), new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kilo, once), new ConvertorOperator(1.0 / 0.028349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kilo, pound), new ConvertorOperator(1.0 / 0.45359237));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(once, gram), new ConvertorOperator(28.349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(once, kilo), new ConvertorOperator(0.028349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(once, pound), new ConvertorOperator(1.0 / 16.0));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(pound, gram), new ConvertorOperator(453.59237));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(pound, kilo), new ConvertorOperator(0.45359237));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(pound, once), new ConvertorOperator(16.0));
    m_rules->setConvertionRules(&m_convertorTable);
    m_rules->setCurrentCategoryId("MASS", Unit::MASS);
    m_rules->convertionRules();
    m_rules->setSourceModel(m_model.get());

    m_rules->columnCount(m_rules->index(0, 0, QModelIndex()));
    // m_rules->removeUnit(m_rules->index(0, 0, QModelIndex()));

    m_rules->makePair(m_rules->index(0, 0, QModelIndex()));

    m_rules->setHeaderData(0, Qt::Vertical, Helper::randomString());
    m_rules->setHeaderData(-1, Qt::Vertical, Helper::randomString());
    m_rules->setHeaderData(1, Qt::Vertical, Helper::randomString());
    m_rules->setHeaderData(100, Qt::Vertical, Helper::randomString());

    m_rules->setHeaderData(0, Qt::Horizontal, Helper::randomString());
    m_rules->setHeaderData(-1, Qt::Horizontal, Helper::randomString());
    m_rules->setHeaderData(1, Qt::Horizontal, Helper::randomString());
    m_rules->setHeaderData(100, Qt::Horizontal, Helper::randomString());

    for(int i= 0; i < m_rules->columnCount(); ++i)
    {
        m_rules->data(m_rules->index(0, i, QModelIndex()), Qt::BackgroundRole);
        m_rules->setData(m_rules->index(0, i, QModelIndex()), Helper::randomString());
        m_rules->flags(m_rules->index(0, i, QModelIndex()));
        // m_rules->setHeaderData(i, Qt::Horizontal, Helper::randomString());
    }
    m_rules->insertUnit();
    m_rules->removeUnit(m_rules->index(m_rules->rowCount() - 1, 0, QModelIndex()));
}

QTEST_MAIN(TestUnitModel);

#include "tst_unitmodel.moc"
