/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
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
#include <QtTest/QtTest>

#include <preferences/preferenceslistener.h>
#include <preferences/preferencesmanager.h>

class PreferencesTest : public QObject, public PreferencesListener
{
    Q_OBJECT

public:
    PreferencesTest();
    void preferencesHasChanged(const QString& pref) override;

private slots:
    void testPreferenceRegisterValue();
    void initTestCase();
    void testNotOverridePreferenceValue();
    void testOverridePreferenceValue();
    void testLambdaFunction();
    void testListener();
    void cleanupTestCase();

private:
    PreferencesManager* m_preferences;
    int m_count= 0;
};

PreferencesTest::PreferencesTest() {}

void PreferencesTest::preferencesHasChanged(const QString& key)
{
    QVERIFY(key == "keyListener");
    QVERIFY(m_preferences->value(key, 18).toInt() == 25);
}

void PreferencesTest::testPreferenceRegisterValue()
{
    m_preferences->registerValue("key", 300);

    QVERIFY(m_preferences->value("key", 400) == 300);
}
void PreferencesTest::testNotOverridePreferenceValue()
{
    m_preferences->registerValue("key1", 300, false);
    QVERIFY(m_preferences->value("key1", 400) == 300);
    m_preferences->registerValue("key1", 100, false);
    QVERIFY(m_preferences->value("key1", 400) == 300);
}
void PreferencesTest::testOverridePreferenceValue()
{
    m_preferences->registerValue("key2", 300);
    QVERIFY(m_preferences->value("key2", 400) == 300);
    m_preferences->registerValue("key2", 100);
    QVERIFY(m_preferences->value("key2", 400) == 100);
}
void PreferencesTest::initTestCase()
{
    m_preferences= PreferencesManager::getInstance();
}

void PreferencesTest::testLambdaFunction()
{
    m_count= 0;
    m_preferences->registerValue("key", 300);
    auto func= [this](QVariant value) {
        QCOMPARE(value.toInt(), 25);
        m_count++;
    };
    m_preferences->registerLambda("key", func);
    m_preferences->registerValue("key", 25);

    QCOMPARE(m_count, 1);
}

void PreferencesTest::testListener()
{
    m_count= 0;
    m_preferences->registerValue("keyListener", 800);
    m_preferences->registerListener("keyListener", this);
    m_preferences->registerValue("keyListener", 25);
}

void PreferencesTest::cleanupTestCase() {}

QTEST_MAIN(PreferencesTest);

#include "tst_preferencestest.moc"
