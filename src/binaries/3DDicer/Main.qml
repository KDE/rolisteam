import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import dicely
import QtQuick3D.Helpers


ApplicationWindow {
    width: 1080/2
    height: 2240/2
    visible: true
    title: stackView.currentItem.title//qsTr("3D Dice")

    color: Theme.windowColor
    onClosing: {
        DiceMainController.saveData();
    }
    function updateLang() {
        if(DiceMainController.lang.length > 0)
            Qt.uiLanguage = DiceMainController.lang
    }

    Component.onCompleted: {
        DiceMainController.themeCtrl = Theme
        updateLang()
    }

    Connections {
        target: DiceMainController
        function onLangChanged() {
            updateLang()
        }
    }

    Connections {
        target: Application
        function onStateChanged() {
            if(Application.state < Application.Inactive)
                DiceMainController.saveData();
        }
    }

    Component {
        id: physics
        PhysicsDicePage {
        }
    }
    Component {
        id: diceRoll
        DiceRollPage {

        }
    }
    Component {
        id: settings
        SettingsPage {

        }
    }
    Component {
        id: macros
        MacrosPage {
        }
    }
    Component {
        id: aliases
        AliasesPage {
        }
    }
    Component {
        id: sheet
        SheetPage {

        }
    }

    function getPageComponent() {
        let res = diceRoll
        switch(DiceMainController.currentPage)
        {
        case DiceMainController.PhysicsPage:
            res = physics;
            break;
        case DiceMainController.CommandsPage:
            res = diceRoll;
            break;
        case DiceMainController.SheetPage:
            res = sheet;
            break;
        case DiceMainController.AliasPage:
            res = aliases;
            break;
        case DiceMainController.MacroPage:
            res = macros;
            break;
        case DiceMainController.SelectContextPage:
            res = profile;
            break;
        case DiceMainController.SettingsPage:
            res = settings;
            break;
        }
        return res
    }

    Connections {
        target: DiceMainController
        function onCurrentPageChanged() {
            stackView.pop()
            stackView.push(getPageComponent())
        }
    }

    Item {
        anchors {
            fill: parent
            topMargin: parent.SafeArea.margins.top
            leftMargin: parent.SafeArea.margins.left
            rightMargin: parent.SafeArea.margins.right
            bottomMargin: parent.SafeArea.margins.bottom
        }
        ColumnLayout {
            id: lyt
            anchors.fill: parent

            TabBar {
                id: tabs
                visible: DiceMainController.show3dMenu
                Layout.fillWidth: true
                background: Rectangle {
                       color: Theme.windowColor
                }

                TabButton {
                       text: qsTr("Dice Rolls")
                       icon.source: "qrc:/assets/diceroll.svg"
                       icon.width: Theme.tabIconSize
                       icon.color: Theme.transparent
                       display: AbstractButton.IconOnly
                       icon.height:Theme.tabIconSize
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.CommandsPage
                }
                TabButton {
                       text: qsTr("3D Dice")
                       icon.source: "qrc:/assets/dice.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: Theme.transparent
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.PhysicsPage
                }
                TabButton {
                       text: qsTr("Macros")
                       icon.source: "qrc:/assets/bookmark.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: Theme.transparent
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.MacroPage
                }
                TabButton {
                       text: qsTr("Aliases")
                       icon.source: "qrc:/assets/aliases.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: Theme.transparent
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.AliasPage
                }
                TabButton {
                       text: qsTr("CharacterSheet")
                       icon.source: "qrc:/assets/sheet2.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: Theme.transparent
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.SheetPage
                }
                TabButton {
                       text: qsTr("Settings")
                       icon.source: "qrc:/assets/settings.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: Theme.transparent
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.SettingsPage
                }
            }
            StackView {
                id: stackView
                Layout.fillWidth: true
                Layout.fillHeight: true
                initialItem: getPageComponent()
            }
        }
    }
}
