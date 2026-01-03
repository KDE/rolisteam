import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import dicely
import QtQuick3D.Helpers
import Walker

ApplicationWindow {
    id: window

    width: 1080/2
    height: 2240/2
    visible: true
    title: stackView.currentItem.title//qsTr("3D Dice")

    color: Theme.windowColor
    property bool mustStartUiTour: false
    onClosing: {
        DiceMainController.saveData();
    }
    function updateLang() {
        if(DiceMainController.lang.length > 0)
            Qt.uiLanguage = DiceMainController.lang
    }

    function startUiTour() {
        if(DiceMainController.currentPage !== DiceMainController.CommandsPage) {
            DiceMainController.currentPage = DiceMainController.CommandsPage
            tabs.currentIndex = 0
            window.mustStartUiTour = true
            return;
        }
        walker.start()
        DiceMainController.uiTour = true;
        window.mustStartUiTour = false
    }


    Component.onCompleted: {
        DiceMainController.themeCtrl = Theme
        updateLang()
        if(DiceMainController.uiTour === DiceMainController.UnDone) {
            startUiTour()
        }
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
                WalkerItem.description: qsTr("Tabbar:\nEach tab manages one main feature.")
                WalkerItem.weight: 10

                TabButton {
                    text: qsTr("Dice Rolls")
                    icon.source: "qrc:/assets/diceroll.svg"
                    icon.width: Theme.tabIconSize
                    icon.color: Theme.transparent
                    display: AbstractButton.IconOnly
                    icon.height:Theme.tabIconSize
                    padding: Theme.margin
                    onClicked: DiceMainController.currentPage = DiceMainController.CommandsPage

                    WalkerItem.description: qsTr("Roll dice:\nAll results are displayed here.")
                    WalkerItem.weight: 100
                    WalkerItem.onEnter: {
                        DiceMainController.currentPage = DiceMainController.CommandsPage
                        tabs.currentIndex = 0
                    }
                }
                TabButton {
                    id: dice3d
                    text: qsTr("3D Dice")
                    icon.source: "qrc:/assets/dice.svg"
                    icon.width: Theme.tabIconSize
                    icon.height:Theme.tabIconSize
                    icon.color: Theme.transparent
                    display: AbstractButton.IconOnly
                    padding: Theme.margin
                    onClicked: DiceMainController.currentPage = DiceMainController.PhysicsPage
                    WalkerItem.description: qsTr("3D Dice:\nRoll your dice in 3D.")
                    WalkerItem.weight: 200
                    WalkerItem.onEnter: {
                        DiceMainController.currentPage = DiceMainController.PhysicsPage
                        tabs.currentIndex = 1
                    }
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
                    WalkerItem.description: qsTr("Macros:\nPreset of often used commands.")
                    WalkerItem.weight: 300
                    WalkerItem.onEnter: {
                        DiceMainController.currentPage = DiceMainController.MacroPage
                        tabs.currentIndex = 2
                    }
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
                    WalkerItem.description: qsTr("Aliases:\nShortcut commands")
                    WalkerItem.weight: 400
                    WalkerItem.onEnter: {
                        DiceMainController.currentPage = DiceMainController.AliasPage
                        tabs.currentIndex = 3
                    }
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
                    WalkerItem.description: qsTr("CharacterSheet:\nStore character's values")
                    WalkerItem.weight: 500
                    WalkerItem.onEnter: {
                        DiceMainController.currentPage = DiceMainController.SheetPage
                        tabs.currentIndex = 4
                    }
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
                    WalkerItem.description: qsTr("Application Settings:\nLanguage, Version…")
                    WalkerItem.weight: 600
                    WalkerItem.onEnter: {
                        DiceMainController.currentPage = DiceMainController.SettingsPage
                        tabs.currentIndex = 5
                    }
                }
            }
            StackView {
                id: stackView
                Layout.fillWidth: true
                Layout.fillHeight: true
                initialItem: getPageComponent()
                onBusyChanged: {
                    if(!stackView.busy && window.mustStartUiTour)
                        startUiTour()
                }
            }
        }

        WalkerItem {
            id: walker
            anchors.fill: parent
            visible: false
            dimOpacity: 0.8
            property int sessionIndex:-1
            onActiveChanged: {
                if(walker.active) {
                    DiceMainController.settingsCtrl.sessions.addSession("UiTour")
                    walker.sessionIndex = DiceMainController.settingsCtrl.sessions.indexFromName("UiTour")
                    DiceMainController.settingsCtrl.currentSessionIndex = walker.sessionIndex
                }
                else
                {
                    DiceMainController.settingsCtrl.sessions.removeSession(walker.sessionIndex)
                    DiceMainController.settingsCtrl.currentSessionIndex = 0
                    DiceMainController.currentPage = DiceMainController.CommandsPage
                    tabs.currentIndex = 0
                }
            }

            Item {
                x: walker.availableRect.x
                y: walker.availableRect.y
                width: walker.availableRect.width
                height: walker.availableRect.height
                Label {
                    id: label
                    text: walker.currentDesc
                    color: "white"
                    anchors.centerIn: parent
                    padding: 20
                    font.pixelSize: 30

                    background: Rectangle {
                        gradient: Gradient {
                            GradientStop { position: 0; color: "transparent" }
                            GradientStop { position: 0.5; color: "black" }
                            GradientStop { position: 1; color: "transparent" }
                        }

                        color:"black"
                        radius: 10
                        opacity: 0.8

                    }
                }
            }

            Rectangle {
                x: walker.borderRect.x-2
                y: walker.borderRect.y-2
                width: walker.borderRect.width+4
                height: walker.borderRect.height+4
                border.color: "red"
                color: "transparent"
                radius: 10
                border.width: 4
            }

            /*ToolButton {
                icon.source: "qrc:/assets/previous.svg"
                icon.color: "transparent"
                icon.width: Theme.tabIconSize
                icon.height:Theme.tabIconSize
                onClicked: walker.previous()
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 50
                anchors.leftMargin: 50
            }*/

            ToolButton {
                icon.source: walker.current + 1 === walker.count ? "qrc:/assets/finish.svg" : "qrc:/assets/next.svg"
                icon.color: "transparent"
                icon.width: Theme.tabIconSize
                icon.height:Theme.tabIconSize
                onClicked:{
                    if(walker.current +1 === walker.count)
                        walker.skip()
                    else
                        walker.next()
                }
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 50
                anchors.rightMargin: 50
            }


        }
    }
}
