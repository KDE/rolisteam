/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   Copyright (C) 2015 by Benoit Lagarde                                  *
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
#include <QList>
#include <QSignalSpy>
#include <QTest>
#include <QtConcurrent>

#include "core/include/controller/gamecontroller.h"
#include "core/include/controller/preferencescontroller.h"
#include "core/include/controller/view_controller/imageselectorcontroller.h"
#include "core/include/media/mediatype.h"
#include "core/include/preferences/preferencesmanager.h"

#include "rwidgets/dialogs/aboutrolisteam.h"
#include "rwidgets/dialogs/campaignintegritydialog.h"
#include "rwidgets/dialogs/campaignproperties.h"
#include "rwidgets/dialogs/connectionretrydialog.h"
#include "rwidgets/dialogs/historyviewerdialog.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include "rwidgets/dialogs/importdatafromcampaigndialog.h"
#include "rwidgets/dialogs/keygeneratordialog.h"
#include "rwidgets/dialogs/newfiledialog.h"
#include "rwidgets/dialogs/persondialog.h"
#include "rwidgets/dialogs/preferencesdialog.h"
#include "rwidgets/dialogs/selectconnectionprofiledialog.h"
#include "rwidgets/dialogs/shortcuteditordialog.h"
#include "rwidgets/dialogs/tipofdayviewer.h"
#include "rwidgets/dialogs/vmapwizzarddialog.h"

#include <helper.h>

class DialogsTest : public QObject
{
    Q_OBJECT

public:
    DialogsTest();

private slots:
    void init();
    void cleanup();

    void aboutRolisteamTest();
    void campIntegrityTest();
    void campPropertyTest();
    void retryTest();
    void historyTest();
    void imageSelectorTest();
    void importDataTest();
    void keyGeneratorTest();
    void newFileDialogTest();
    void personDialogTest();
    void prefDialogTest();
    void selectProfilTest();
    void shortCutTest();
    void tipOfTheDayTest();
    void mapWizzardTest();

private:
    std::unique_ptr<AboutRolisteam> m_aboutRolisteam;
    std::unique_ptr<campaign::CampaignIntegrityDialog> m_campIntegrityDialog;
    std::unique_ptr<campaign::CampaignIntegrityController> m_campIntegrityCtrl;
    std::unique_ptr<CampaignProperties> m_campProperty;
    std::unique_ptr<ConnectionRetryDialog> m_retry;
    std::unique_ptr<HistoryViewerDialog> m_history;
    std::unique_ptr<ImageSelectorDialog> m_imageSelector;
    std::unique_ptr<ImportDataFromCampaignDialog> m_importData;
    std::unique_ptr<KeyGeneratorDialog> m_keyGenerator;
    std::unique_ptr<NewFileDialog> m_newFileDialog;
    std::unique_ptr<PersonDialog> m_personDialog;
    std::unique_ptr<PreferencesDialog> m_prefDialog;
    std::unique_ptr<SelectConnectionProfileDialog> m_selectProfil;
    std::unique_ptr<ShortCutEditorDialog> m_shortCut;
    std::unique_ptr<TipOfDayViewer> m_tipOfTheDay;
    std::unique_ptr<MapWizzardDialog> m_mapWizzard;
};
DialogsTest::DialogsTest() {}
void DialogsTest::init()
{
    m_aboutRolisteam.reset(new AboutRolisteam);
    m_retry.reset(new ConnectionRetryDialog);
    m_keyGenerator.reset(new KeyGeneratorDialog);
    m_personDialog.reset(new PersonDialog);
    m_shortCut.reset(new ShortCutEditorDialog);
}

void DialogsTest::cleanup()
{
    if(m_aboutRolisteam)
        m_aboutRolisteam->close();

    if(m_campIntegrityDialog)
        m_campIntegrityDialog->close();

    if(m_campProperty)
        m_campProperty->close();

    if(m_retry)
        m_retry->close();

    if(m_history)
        m_history->close();

    if(m_imageSelector)
        m_imageSelector->close();

    if(m_importData)
        m_importData->close();

    if(m_keyGenerator)
        m_keyGenerator->close();

    if(m_newFileDialog)
        m_newFileDialog->close();

    if(m_personDialog)
        m_personDialog->close();

    if(m_prefDialog)
        m_prefDialog->close();

    if(m_selectProfil)
    {
        m_selectProfil->hide();
        // m_selectProfil->deleteLater();
        m_selectProfil.release();
    }

    if(m_shortCut)
        m_shortCut->close();

    if(m_tipOfTheDay)
        m_tipOfTheDay->close();

    if(m_mapWizzard)
        m_mapWizzard->close();
}

void DialogsTest::aboutRolisteamTest()
{
    m_aboutRolisteam->open();
}
void DialogsTest::campIntegrityTest()
{
    QStringList missingFiles;
    QStringList unmanagedFile;
    m_campIntegrityCtrl.reset(new campaign::CampaignIntegrityController(missingFiles, unmanagedFile, nullptr));
    m_campIntegrityDialog.reset(new campaign::CampaignIntegrityDialog(m_campIntegrityCtrl.get()));
    m_campIntegrityDialog->open();
}
void DialogsTest::campPropertyTest()
{
    campaign::Campaign campaign;
    m_campProperty.reset(new CampaignProperties(&campaign, nullptr));

    for(auto v : {CampaignProperties::Tab::Properties, CampaignProperties::Tab::Dice, CampaignProperties::Tab::States})
        m_campProperty->setCurrentTab(v);

    m_campProperty->open();
}
void DialogsTest::retryTest()
{
    QSignalSpy spy(m_retry.get(), &ConnectionRetryDialog::tryConnection);
    m_retry->setTimeOut(100);
    m_retry->setTimeOut(100);
    m_retry->setCounter(10);
    m_retry->setCounter(10);
    QCOMPARE(m_retry->timeOut(), 100);
    QCOMPARE(m_retry->counter(), 10);
    m_retry->startTimer();
    m_retry->open();

    spy.wait(100 * 11);
    QCOMPARE(spy.count(), 1);
}
void DialogsTest::historyTest()
{
    auto model= new history::HistoryModel(this);
    m_history.reset(new HistoryViewerDialog(model));
    m_history->open();
}
void DialogsTest::imageSelectorTest()
{
    ImageSelectorController ctrl;
    m_imageSelector.reset(new ImageSelectorDialog(&ctrl));
    m_imageSelector->open();
}
void DialogsTest::importDataTest()
{
    m_importData.reset(new ImportDataFromCampaignDialog(Helper::randomString()));
    m_importData->open();
}
void DialogsTest::keyGeneratorTest()
{
    m_keyGenerator->open();
}
void DialogsTest::newFileDialogTest()
{
    QList<Core::ContentType> list{Core::ContentType::SHAREDNOTE, Core::ContentType::NOTES, Core::ContentType::MINDMAP};

    for(auto l : list)
    {
        m_newFileDialog.reset(new NewFileDialog(l));
        m_newFileDialog->show();
        QCOMPARE(m_newFileDialog->type(), l);
        m_newFileDialog->name();
        m_newFileDialog->url();
    }
}
void DialogsTest::personDialogTest()
{
    auto t= Helper::randomString();
    auto n= Helper::randomString();
    auto c= Helper::randomColor();
    auto u= Helper::randomUrl();
    m_personDialog->edit(t, n, c, u.toString());

    QCOMPARE(m_personDialog->getName(), n);
    QCOMPARE(m_personDialog->getColor(), c);
    QCOMPARE(m_personDialog->windowTitle(), t);
    QCOMPARE(m_personDialog->getAvatarUri(), u.toString());

    // auto f= QtConcurrent::run([this]() { m_personDialog->openImage(); });

    m_personDialog->reset();
    m_personDialog->clickOnBar(new QPushButton(m_personDialog.get()));
    // m_personDialog->clickOnBar(new QPushButton(QDialogButtonBox::ResetRole));

    m_personDialog->open();
}
void DialogsTest::prefDialogTest()
{
    PreferencesController ctrl;
    m_prefDialog.reset(new PreferencesDialog(&ctrl));

    for(auto v : {PreferencesDialog::PreferenceTab::General, PreferencesDialog::PreferenceTab::Player,
                  PreferencesDialog::PreferenceTab::Themes, PreferencesDialog::PreferenceTab::Themes})
    {
        m_prefDialog->show(v);
        m_prefDialog->backgroundChanged();
        m_prefDialog->manageHeartBeat();
        m_prefDialog->manageMessagingPref();
        m_prefDialog->updateTranslationPref();
        m_prefDialog->accept();
        m_prefDialog->hide();
    }
}
void DialogsTest::selectProfilTest()
{
    GameController ctrl(Helper::randomString(), nullptr);

    m_selectProfil.reset(new SelectConnectionProfileDialog(&ctrl));
    m_selectProfil->setArgumentProfile(Helper::randomString(), Helper::generate<int>(1024, 35000),
                                       Helper::randomData());
    // auto f= QtConcurrent::run([this]() { m_selectProfil->openImage(); });
    m_selectProfil->open();
}
void DialogsTest::shortCutTest()
{
    m_shortCut->setModel(nullptr);
    QCOMPARE(m_shortCut->model(), nullptr);
    m_shortCut->open();
}
void DialogsTest::tipOfTheDayTest()
{
    m_tipOfTheDay.reset(new TipOfDayViewer(Helper::randomString(), Helper::randomString(), Helper::randomString()));
    m_tipOfTheDay->open();
}
void DialogsTest::mapWizzardTest()
{
    PreferencesManager ctrl(Helper::randomString(), Helper::randomString());
    m_mapWizzard.reset(new MapWizzardDialog(&ctrl));

    m_mapWizzard->name();
    m_mapWizzard->backgroundColor();
    m_mapWizzard->gridSize();
    m_mapWizzard->gridColor();
    m_mapWizzard->pattern();
    m_mapWizzard->permission();
    m_mapWizzard->visibility();
    m_mapWizzard->unit();
    m_mapWizzard->scale();
    m_mapWizzard->updateUI();

    m_mapWizzard->open();
}

QTEST_MAIN(DialogsTest);

#include "tst_dialogs.moc"
