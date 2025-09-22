#include "dicemaincontroller.h"

#include <QtConcurrent>

#include "diceparser/dicealias.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/modelhelper.h"
#include <QJsonArray>

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
    return m_profileCount;
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

            if(!m_parser->parseLine(cmd))
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

void DiceMainController::saveData()
{
    QSettings settings("Rolisteam", "DiceRoller");

    // settings.setValue("sessionCount", m_settingsCtrl->sessionCount());
    settings.setValue("sessionNames", m_settingsCtrl->sessions()->sessionNames()); // TODO change me
    settings.setValue("currentSession", m_settingsCtrl->currentSessionIndex());
    for(int i= 0; i < m_settingsCtrl->sessionCount(); ++i)
    {
        m_settingsCtrl->setCurrentSessionIndex(i);
        auto name= m_settingsCtrl->sessionName();
        settings.beginGroup(name);

        auto aliasJson= IOHelper::jsonArrayToByteArray(campaign::FileSerializer::dicesToArray(m_aliases->aliases()));
        settings.setValue("aliases", aliasJson);

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
            auto sheet= IOHelper::jsonArrayToByteArray(array);
            settings.setValue("sheet", sheet);
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
            auto sheet= IOHelper::jsonArrayToByteArray(array);
            settings.setValue("macros", sheet);
        }
        settings.endGroup();
    }

    settings.sync();
}

void DiceMainController::loadData()
{
    QSettings settings("Rolisteam", "DiceRoller");
    auto names= settings.value("sessionNames", {"default"}).toStringList();

    for(const auto& n : std::as_const(names))
    {
        settings.beginGroup(n);
        auto json= settings.value("aliases").toByteArray();
        auto sheetjson= settings.value("sheet").toByteArray();
        auto macrosjson= settings.value("macros").toByteArray();

        ModelHelper::fetchDiceModel(IOHelper::byteArrayToJsonArray(json), m_aliases.get());

        {
            auto array= IOHelper::byteArrayToJsonArray(sheetjson);
            for(auto const& r : std::as_const(array))
            {
                auto obj= r.toObject();
                m_propertiesModel->addField(obj["key"].toString(), obj["value"].toString());
            }
        }

        {
            auto array= IOHelper::byteArrayToJsonArray(macrosjson);
            for(auto const& r : std::as_const(array))
            {
                auto obj= r.toObject();
                m_macros->addMacro(obj["name"].toString(), obj["command"].toString());
            }
        }

        m_settingsCtrl->sessions()->addSession(n);
        settings.endGroup();
    }

    m_settingsCtrl->setCurrentSessionIndex(settings.value("currentSession", 0).toInt());
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

void DiceMainController::setCurrentPanel(const PanelMode &newCurrentPanel)
{
    if (m_currentPanel == newCurrentPanel)
        return;
    m_currentPanel = newCurrentPanel;
    emit currentPanelChanged();
}
