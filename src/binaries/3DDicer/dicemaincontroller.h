#ifndef DICEMAINCONTROLLER_H
#define DICEMAINCONTROLLER_H

#include "dice3dcontroller.h"
#include "diceparser/diceparser.h"
#include "macromodel.h"
#include "model/dicealiasmodel.h"
#include "propertiesmodel.h"
#include "rollmodel.h"
#include "settingcontroller.h"
#include <QObject>
#include <QQmlEngine>
#include <memory>

class DiceMainController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    // clang-format off
    Q_PROPERTY(DiceMainController::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged FINAL)
    Q_PROPERTY(RollModel* model READ model CONSTANT)
    Q_PROPERTY(int profileCount READ profileCount NOTIFY profileCountChanged FINAL)
    Q_PROPERTY(QString errorHumanReadable READ errorHumanReadable WRITE setErrorHumanReadable NOTIFY errorHumanReadableChanged FINAL)
    Q_PROPERTY(Dice3DController* dice3dCtrl READ dice3dCtrl CONSTANT FINAL)
    Q_PROPERTY(QSize dice3dSize READ dice3dSize WRITE setDice3dSize NOTIFY dice3dSizeChanged FINAL)
    Q_PROPERTY(DiceAliasModel* aliases READ aliases CONSTANT)
    Q_PROPERTY(SettingController* settingsCtrl READ settingsCtrl CONSTANT FINAL)
    Q_PROPERTY(PropertiesModel* propertiesModel READ propertiesModel CONSTANT FINAL)
    Q_PROPERTY(MacrosModel* macros READ macros CONSTANT)
    Q_PROPERTY(DiceMainController::PanelMode currentPanel READ currentPanel WRITE setCurrentPanel NOTIFY currentPanelChanged FINAL)
    // clang-format on
public:
    enum Page
    {
        PhysicsPage,
        CommandsPage,
        SheetPage,
        AliasPage,
        MacroPage,
        SelectContextPage,
        SettingsPage
    };
    Q_ENUM(Page)

    enum PanelMode
    {
        TypeCommand,
        MacroPanel
    };
    Q_ENUM(PanelMode)
    explicit DiceMainController(QObject* parent= nullptr);
    void setCurrentPage(DiceMainController::Page newCurrentPage);

    RollModel* model() const;
    int profileCount() const;

    QString errorHumanReadable() const;
    void setErrorHumanReadable(const QString& newErrorHumanReadable);

    Dice3DController* dice3dCtrl() const;

    QSize screenSize() const;
    void setScreenSize(const QSize& newScreenSize);

    QSize dice3dSize() const;
    void setDice3dSize(const QSize& newDice3dSize);

    DiceAliasModel* aliases() const;

    SettingController* settingsCtrl() const;
    void setSettingsCtrl(SettingController* newSettingsCtrl);

    PropertiesModel* propertiesModel() const;
    MacrosModel* macros() const;

    DiceMainController::Page currentPage() const;
    PanelMode currentPanel() const;
    void setCurrentPanel(const PanelMode& newCurrentPanel);

public slots:
    void runCommand(const QString& cmd);
    void addAlias();
    void saveData();
    void loadData();

signals:
    void currentPageChanged();
    void profileCountChanged();
    void errorHumanReadableChanged();
    void dice3dSizeChanged();

    void settingsCtrlChanged();

    void propertiesModelChanged();

    void currentPanelChanged();

private:
    std::unique_ptr<RollModel> m_model;
    std::unique_ptr<DiceParser> m_parser;
    std::unique_ptr<Dice3DController> m_dice3DCtrl;
    std::unique_ptr<DiceAliasModel> m_aliases;
    std::unique_ptr<SettingController> m_settingsCtrl;
    std::unique_ptr<PropertiesModel> m_propertiesModel;
    DiceMainController::Page m_currentPage{CommandsPage};
    int m_profileCount;
    QString m_errorHumanReadable;
    QSize m_dice3dSize;
    std::unique_ptr<MacrosModel> m_macros;
    PanelMode m_currentPanel;
};

#endif // DICEMAINCONTROLLER_H
