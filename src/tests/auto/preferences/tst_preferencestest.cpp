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
#include <QTest>

#include "data/rolisteamtheme.h"
#include "test_root_path.h"
#include <controller/preferencescontroller.h>
#include <helper.h>
#include <media/mediatype.h>
#include <preferences/preferenceslistener.h>
#include <preferences/preferencesmanager.h>

class PreferencesTest : public QObject, public PreferencesListener
{
    Q_OBJECT

public:
    PreferencesTest();
    void preferencesHasChanged(const QString& pref) override;

private slots:
    void init();
    void cleanup();
    void testPreferenceRegisterValue();
    void testNotOverridePreferenceValue();
    void testOverridePreferenceValue();
    void testLambdaFunction();
    void testListener();
    void cleanupTestCase();

private:
    PreferencesManager* m_preferences;
    std::unique_ptr<PreferencesController> m_ctrl;
    int m_count= 0;
};

PreferencesTest::PreferencesTest() {}
void PreferencesTest::cleanup()
{
    delete m_preferences;
}
void PreferencesTest::init()
{
    m_preferences= new PreferencesManager("Rolisteam", "tests");
    m_ctrl.reset(new PreferencesController);

    m_ctrl->setPreferencesManager(m_preferences);

    m_ctrl->loadPreferences();
    m_ctrl->savePreferences();

    auto model= m_ctrl->themeModel();
    QCOMPARE(model->rowCount(), 0);

    auto theme= new RolisteamTheme();
    theme->setRemovable(true);
    auto uuid= theme->uuid();

    model->addTheme(theme);
    QCOMPARE(model->rowCount(), 1);

    auto path= Helper::randomString();
    m_ctrl->exportData(path);
    m_ctrl->importData(path);

    m_ctrl->exportData({});
    m_ctrl->importData({});

    m_ctrl->setCurrentThemeIndex(0);

    QCOMPARE(m_ctrl->currentEditableTheme(), theme);
    m_ctrl->dupplicateTheme(0, false);
    m_ctrl->dupplicateTheme(1, true);
    m_ctrl->setCurrentThemeIndex(0);

    QCOMPARE(m_ctrl->theme(uuid), theme);

    auto tName= Helper::randomString();
    m_ctrl->setCurrentThemeTitle(tName);
    QCOMPARE(m_ctrl->themeName(0), tName);

    m_ctrl->setDiceHighLightColor(Helper::randomColor());

    auto color= Helper::randomColor();
    m_ctrl->setColorCurrentTheme(color, 0);

    auto css= Helper::randomString();
    m_ctrl->setCurrentThemeCss(css);
    // QCOMPARE(m_ctrl->themeName(0), css);

    auto title= Helper::randomString();
    m_ctrl->setCurrentThemeTitle(title);

    m_ctrl->setCurrentThemeBackground(Helper::randomString(), Helper::generate<int>(0, 100), Helper::randomColor());

    std::array<Core::MediaType, 5> medias{Core::MediaType::CharacterSheetFile, Core::MediaType::MindmapFile,
                                          Core::MediaType::TextFile, Core::MediaType::MarkdownFile,
                                          Core::MediaType::PdfFile};

    for(auto p : medias)
    {
        m_ctrl->externalEditorFor(p);
        m_ctrl->paramsFor(p);
    }

    auto langs= m_ctrl->languageModel();
    m_ctrl->setCurrentLangIndex(Helper::generate<int>(0, langs->rowCount() - 1));

    m_ctrl->setCurrentLangIndex(Helper::generate<int>(-100, -2));

    m_ctrl->setCurrentLangIndex(Helper::generate<int>(langs->rowCount() + 2, (langs->rowCount() + 2) * 200));
}

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

void PreferencesTest::testLambdaFunction()
{
    m_count= 0;
    auto expected= 300;
    m_preferences->registerValue("key", expected);
    auto func= [this, &expected](QVariant value)
    {
        QCOMPARE(value.toInt(), expected);
        m_count++;
    };
    m_preferences->registerLambda("key", func);
    expected= 25;
    m_preferences->registerValue("key", expected);

    QCOMPARE(m_count, 2);
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
