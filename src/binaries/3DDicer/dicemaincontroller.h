#ifndef DICEMAINCONTROLLER_H
#define DICEMAINCONTROLLER_H

#include "dice3dcontroller.h"
#include "diceparser/diceparser.h"
#include "rollmodel.h"
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
    // clang-format on
public:
    enum Page
    {
        PhysicsPage,
        CommandsPage,
        SheetPage,
        AliasPage,
        SelectContextPage,
        SettingsPage
    };
    Q_ENUM(Page)
    explicit DiceMainController(QObject* parent= nullptr);

    DiceMainController::Page currentPage() const;
    void setCurrentPage(DiceMainController::Page newCurrentPage);

    RollModel* model() const;
    int profileCount() const;

    QString errorHumanReadable() const;
    void setErrorHumanReadable(const QString& newErrorHumanReadable);

    Dice3DController* dice3dCtrl() const;

    QSize screenSize() const;
    void setScreenSize(const QSize& newScreenSize);

    QSize dice3dSize() const;
    void setDice3dSize(const QSize &newDice3dSize);

public slots:
    void runCommand(const QString& cmd);

signals:
    void currentPageChanged();
    void profileCountChanged();
    void errorHumanReadableChanged();

    void screenSizeChanged();

    void dice3dSizeChanged();

private:
    std::unique_ptr<RollModel> m_model;
    std::unique_ptr<DiceParser> m_parser;
    std::unique_ptr<Dice3DController> m_dice3DCtrl;
    DiceMainController::Page m_currentPage{CommandsPage};
    int m_profileCount;
    QString m_errorHumanReadable;
    QSize m_dice3dSize;
};

#endif // DICEMAINCONTROLLER_H
