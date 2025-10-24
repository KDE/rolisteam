#include "dicemaincontroller.h"

#include <QtConcurrent>

#include "dicelyverse/version.h"
#include "diceparser/dicealias.h"
#include "worker/fileserializer.h"
#include <QJsonArray>
#include <QMap>
#include <QStandardPaths>
#include <QVariant>

QSettings* computeSettingsPath()
{
#ifdef Q_OS_ANDROID
    const QString configfile("config.ini");
    auto path= QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    auto filePath= QStringLiteral("%1/%2").arg(path, configfile);
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly))
    {
        qDebug() << "initfile: " << file.readAll();
    }
    else
        qDebug() << "Can't read initfile" << filePath;
    return new QSettings(QStringLiteral("%1/%2").arg(path, configfile));
#else
    return new QSettings(QSettings::UserScope, "Rolisteam", "DicelyVerse");
#endif
}

DiceMainController::DiceMainController(QObject* parent)
    : QObject{parent}
    , m_model{new RollModel}
    , m_parser{new DiceParser}
    , m_dice3DCtrl{new Dice3DController}
    , m_aliases{new DiceAliasModel}
    , m_settingsCtrl{new SettingController}
    , m_propertiesModel{new PropertiesModel}
    , m_macros{new MacrosModel}
{

    loadData();
    m_dice3DCtrl->setMuted(true);
    connect(m_settingsCtrl.get(), &SettingController::currentSessionIndexChanged, this,
            &DiceMainController::loadFromCurrentProfile);
    connect(m_settingsCtrl.get(), &SettingController::sessionCountChanged, this,
            &DiceMainController::profileCountChanged);

    connect(m_dice3DCtrl.get(), &Dice3DController::countChanged, this, &DiceMainController::saveData);
    connect(m_dice3DCtrl.get(), &Dice3DController::colorChanged, this, &DiceMainController::saveData);

    connect(m_aliases.get(), &DiceAliasModel::aliasAdded, this, &DiceMainController::saveData);
    connect(m_aliases.get(), &DiceAliasModel::aliasRemoved, this, &DiceMainController::saveData);
    connect(m_aliases.get(), &DiceAliasModel::aliasMoved, this, &DiceMainController::saveData);

    connect(m_propertiesModel.get(), &PropertiesModel::countChanged, this, &DiceMainController::saveData);

    connect(m_macros.get(), &MacrosModel::countChanged, this, &DiceMainController::saveData);
    connect(m_settingsCtrl.get(), &SettingController::sessionCountChanged, this, &DiceMainController::saveData);

    QDirIterator it(QString(":/qt/qml/dicely/i18n/"), QDirIterator::Subdirectories);
    using LangEntry= QMap<Qt::ItemDataRole, QVariant>;

    QList<LangEntry> res;
    while(it.hasNext())
    {
        auto code= it.next().replace(".qm", "").replace(":/qt/qml/dicely/i18n/qml_", "");
        auto locale= QLocale(QLocale::codeToLanguage(code));
        res << LangEntry({{Qt::DisplayRole, locale.nativeLanguageName()},
                          {Qt::UserRole, code}}); // QLocale::languageToString(QLocale::codeToLanguage(code))
    }

    m_langModel.reset(new QRangeModel(res));
    m_langModel->setRoleNames({{Qt::UserRole, "code"}, {Qt::DisplayRole, "display"}});
}

DiceMainController::Page DiceMainController::currentPage() const
{
    return m_currentPage;
}

void DiceMainController::setCurrentPage(DiceMainController::Page newCurrentPage)
{
    if(m_currentPage == newCurrentPage)
        return;
    m_currentPage= newCurrentPage;
    emit currentPageChanged();
}

RollModel* DiceMainController::model() const
{
    return m_model.get();
}

int DiceMainController::profileCount() const
{
    return m_settingsCtrl->sessionCount();
}

void DiceMainController::runCommand(const QString& cmd)
{
    auto it= QtConcurrent::run(
        [this, cmd]()
        {
            auto const& aliases= m_aliases->aliases();
            m_parser->cleanAliases();
            int i= 0;
            std::for_each(std::begin(aliases), std::end(aliases),
                          [this, &i](const std::unique_ptr<DiceAlias>& alias)
                          {
                              m_parser->insertAlias(alias.get(), i);
                              ++i;
                          });

            m_parser->setVariableDictionary(m_propertiesModel->dictionary());

            if(!m_parser->parseLine(cmd.trimmed()))
            {
                setErrorHumanReadable(m_parser->humanReadableError());
                return;
            }

            m_parser->start();

            auto error= m_parser->humanReadableError();
            if(!error.isEmpty())
            {
                setErrorHumanReadable(m_parser->humanReadableError());
                return;
            }

            QByteArray result
                = m_parser
                      ->resultAsJSon(
                          [](const QString& value, const QString& color, bool highlight)
                          {
                              QString resultTmp= value;
                              bool hasColor= !color.isEmpty();
                              QString style;
                              if(hasColor)
                              {
                                  style+= QStringLiteral("color:%1;").arg(color);
                              }
                              if(highlight)
                              {
                                  if(style.isEmpty())
                                      style
                                          += QStringLiteral("color:%1;")
                                                 .arg(
                                                     "red"); // default color must get the value from the setting object
                                  style+= QStringLiteral("font-weight:bold;");
                              }
                              if(!style.isEmpty())
                                  resultTmp= QString("<span style=\"%2\">%1</span>").arg(value, style);
                              return resultTmp;
                          })
                      .toUtf8();

            QMetaObject::invokeMethod(m_model.get(), &RollModel::addRoll, result);
        });
}

QString DiceMainController::errorHumanReadable() const
{
    return m_errorHumanReadable;
}

void DiceMainController::setErrorHumanReadable(const QString& newErrorHumanReadable)
{
    if(m_errorHumanReadable == newErrorHumanReadable)
        return;
    m_errorHumanReadable= newErrorHumanReadable;
    emit errorHumanReadableChanged();
}

Dice3DController* DiceMainController::dice3dCtrl() const
{
    return m_dice3DCtrl.get();
}

QSize DiceMainController::dice3dSize() const
{
    return m_dice3DCtrl->size();
}

void DiceMainController::setDice3dSize(const QSize& newDice3dSize)
{
    m_dice3DCtrl->setSize(newDice3dSize);
}

DiceAliasModel* DiceMainController::aliases() const
{
    return m_aliases.get();
}
void DiceMainController::addAlias()
{
    DiceAlias a("", "");
    m_aliases->appendAlias(std::move(a));
}

void DiceMainController::saveInCurrentProfile()
{
    auto current= m_settingsCtrl->currentSession();
    if(!current)
        return;

    auto aliasJson
        = dice3D::FileSerializer::jsonArrayToByteArray(dice3D::FileSerializer::dicesToArray(m_aliases->aliases()));

    current->setAliases(aliasJson);
    {
        auto const& infos= m_propertiesModel->infos();
        QJsonArray array;
        for(auto const& i : infos)
        {
            QJsonObject obj;
            obj["key"]= i.key;
            obj["value"]= i.value;
            array.append(obj);
        }
        auto sheet= dice3D::FileSerializer::jsonArrayToByteArray(array);
        current->setSheets(sheet);
    }

    {
        auto const& macros= m_macros->macros();
        QJsonArray array;
        for(auto const& i : macros)
        {
            QJsonObject obj;
            obj["name"]= i.name;
            obj["command"]= i.command;
            array.append(obj);
        }
        auto sheet= dice3D::FileSerializer::jsonArrayToByteArray(array);
        current->setMacros(sheet);
    }

    {
        auto json= dice3D::FileSerializer::buildDice3dData(m_dice3DCtrl.get());
        current->setDice3D(json);
    }
}

void DiceMainController::loadFromCurrentProfile()
{
    auto current= m_settingsCtrl->currentSession();
    if(!current)
        return;

    m_loading= true;
    m_propertiesModel->clear();
    m_macros->clear();
    m_aliases->clear();
    m_dice3DCtrl->reset();

    auto array= dice3D::FileSerializer::byteArrayToJsonArray(current->sheets());
    for(auto const& r : std::as_const(array))
    {
        auto obj= r.toObject();
        auto key= obj["key"].toString();
        auto value= obj["value"].toString();
        if(key.isEmpty() && value.isEmpty())
            continue;
        m_propertiesModel->addField(key, value);
    }

    auto macro= dice3D::FileSerializer::byteArrayToJsonArray(current->macros());
    for(auto const& r : std::as_const(macro))
    {
        auto obj= r.toObject();
        auto key= obj["name"].toString();
        auto value= obj["command"].toString();
        if(key.isEmpty() && value.isEmpty())
            continue;
        m_macros->addMacro(key, value);
    }

    dice3D::FileSerializer::fetchDiceModel(dice3D::FileSerializer::byteArrayToJsonArray(current->aliases()),
                                           m_aliases.get());
    dice3D::FileSerializer::fetchDice3d(m_dice3DCtrl.get(), current->dice3D());

    if(m_macros->rowCount() == 0)
        setCurrentPanel(TypeCommand);

    m_loading= false;
}

void DiceMainController::saveData()
{
    if(m_loading)
        return;

    saveInCurrentProfile();

    std::unique_ptr<QSettings> settings(computeSettingsPath());

    settings->setValue("sessionNames", m_settingsCtrl->sessions()->sessionNames()); // TODO change me
    settings->setValue("currentSession", m_settingsCtrl->currentSessionIndex());
    settings->setValue("lang", m_lang);
    if(m_themeCtrl)
        settings->setValue("darkMode", m_themeCtrl->darkMode());

    for(int i= 0; i < m_settingsCtrl->sessionCount(); ++i)
    {
        auto session= m_settingsCtrl->sessions()->session(i);
        if(!session)
            continue;

        auto name= session->name();
        settings->beginGroup(name);
        settings->setValue("aliases", session->aliases());
        settings->setValue("sheet", session->sheets());
        settings->setValue("macros", session->macros());
        settings->setValue("dice3d", session->dice3D());
        settings->endGroup();
    }

    settings->sync();
}

void DiceMainController::loadData()
{
    std::unique_ptr<QSettings> settings(computeSettingsPath());
    auto names= settings->value("sessionNames", {tr("default")}).toStringList();
    setLang(settings->value("lang", QString()).toString());

    for(const auto& n : std::as_const(names))
    {
        settings->beginGroup(n);
        auto json= settings->value("aliases").toByteArray();
        auto sheetjson= settings->value("sheet").toByteArray();
        auto macrosjson= settings->value("macros").toByteArray();
        auto dice3djson= settings->value("dice3d").toByteArray();

        m_settingsCtrl->sessions()->addSession(n);
        auto session= m_settingsCtrl->sessions()->session(n);
        session->setAliases(json);
        session->setSheets(sheetjson);
        session->setMacros(macrosjson);
        session->setDice3D(dice3djson);
        settings->endGroup();
    }

    if(names.isEmpty())
        m_settingsCtrl->sessions()->addSession(tr("default"));

    m_settingsCtrl->setCurrentSessionIndex(settings->value("currentSession", 0).toInt());
    loadFromCurrentProfile();
    if(m_themeCtrl)
        m_themeCtrl->setDarkMode(settings->value("darkMode", false).toBool());
    else
        m_darkMode= settings->value("darkMode", false).toBool();
}

SettingController* DiceMainController::settingsCtrl() const
{
    return m_settingsCtrl.get();
}

PropertiesModel* DiceMainController::propertiesModel() const
{
    return m_propertiesModel.get();
}

MacrosModel* DiceMainController::macros() const
{
    return m_macros.get();
}

DiceMainController::PanelMode DiceMainController::currentPanel() const
{
    return m_currentPanel;
}

void DiceMainController::setCurrentPanel(const PanelMode& newCurrentPanel)
{
    if(m_currentPanel == newCurrentPanel)
        return;
    m_currentPanel= newCurrentPanel;
    emit currentPanelChanged();
}

bool DiceMainController::show3dMenu() const
{
    return m_show3dMenu;
}

void DiceMainController::setShow3dMenu(bool newShow3dMenu)
{
    if(m_show3dMenu == newShow3dMenu)
        return;
    m_show3dMenu= newShow3dMenu;
    emit show3dMenuChanged();
}

QString DiceMainController::version() const
{
    return DicelyVerse::FULL_VERSION;
}

QString DiceMainController::hashVersion() const
{
    return DicelyVerse::VERSION_SHA1;
}

QString DiceMainController::dateVersion() const
{
    return DicelyVerse::VERSION_DATE;
}

bool DiceMainController::darkMode() const
{
    return m_darkMode;
}

void DiceMainController::setDarkMode(bool newDarkMode)
{
    if(m_darkMode == newDarkMode)
        return;
    m_darkMode= newDarkMode;
    emit darkModeChanged();
}

ThemeController* DiceMainController::themeCtrl() const
{
    return m_themeCtrl;
}

void DiceMainController::setThemeCtrl(ThemeController* newThemeCtrl)
{
    if(m_themeCtrl == newThemeCtrl)
        return;
    m_themeCtrl= newThemeCtrl;
    if(m_themeCtrl)
        m_themeCtrl->setDarkMode(m_darkMode);
    emit themeCtrlChanged();
}

QRangeModel* DiceMainController::langModel() const
{
    return m_langModel.get();
}

QString DiceMainController::lang() const
{
    return m_lang;
}

void DiceMainController::setLang(const QString& newLang)
{
    if(m_lang == newLang)
        return;
    m_lang= newLang;
    emit langChanged();
}
