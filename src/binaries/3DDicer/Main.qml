import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import treeDicer
import QtQuick3D.Helpers


ApplicationWindow {
    width: 1080/2
    height: 2240/2
    visible: true
    title: stackView.currentItem.title//qsTr("3D Dice")

    onClosing: {
        DiceMainController.saveData();
    }
    Connections {
        target: Application
        function onStateChanged() {
            if(Application.state < Application.Inactive)
                DiceMainController.saveData();
        }
    }

    ProceduralSkyTextureData {
        id: skyTexture
        sunLongitude: -115
        groundBottomColor : Qt.rgba(0.5, 0.5, 0.5, 0.5)
        //onDestroyed: console.log("SkyTexture")
    }

    Component.onCompleted: {
        DiceMainController.skyTextureData = skyTexture
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
        anchors.fill: parent
        ColumnLayout {
            id: lyt
            anchors.fill: parent

            TabBar {
                id: tabs
                visible: DiceMainController.show3dMenu
                Layout.fillWidth: true
                //Layout.preferredHeight: Theme.iconSize * 2 + (Theme.margin * 2)
                TabButton {
                       text: qsTr("Dice Rolls")
                       icon.source: "qrc:/assets/diceroll.svg"
                       icon.width: Theme.tabIconSize
                       icon.color: "transparent"
                       display: AbstractButton.IconOnly
                       icon.height:Theme.tabIconSize
                       //indicator: Item{}
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.CommandsPage
                }
                TabButton {
                       text: qsTr("Dice 3D")
                       icon.source: "qrc:/assets/dice.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: "transparent"
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.PhysicsPage
                }
                TabButton {
                       text: qsTr("Macros")
                       icon.source: "qrc:/assets/bookmark.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: "transparent"
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.MacroPage
                }
                TabButton {
                       text: qsTr("Aliases")
                       icon.source: "qrc:/assets/aliases.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: "transparent"
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.AliasPage
                }
                TabButton {
                       text: qsTr("CharacterSheet")
                       icon.source: "qrc:/assets/sheet2.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: "transparent"
                       display: AbstractButton.IconOnly
                       padding: Theme.margin
                       onClicked: DiceMainController.currentPage = DiceMainController.SheetPage
                }
                TabButton {
                       text: qsTr("Settings")
                       icon.source: "qrc:/assets/settings.svg"
                       icon.width: Theme.tabIconSize
                       icon.height:Theme.tabIconSize
                       icon.color: "transparent"
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
