import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import dicely
import Walker

Panel {
    id: root
    title: qsTr("Settings")
    property bool hasSelection: group.checkedButton !== null
    Flickable {
        id: flick
        Layout.fillHeight: true
        Layout.fillWidth: true
        //clip: true
        ButtonGroup {
            id: group
            buttons: [profile.button, appSettings.button, about.button]
            property Item previous


            onClicked: button => {
                if(group.previous === button && group.checkState != Qt.Unchecked) {
                    group.checkState = Qt.Unchecked
                    group.previous = null
                    return
                }

                previous = button
            }
        }
        ColumnLayout {
            id: column
            width: flick.width
            spacing: Theme.spacing
            ExpansionPanel {
                id: profile
                text: qsTr("Select Profile")
                Layout.fillWidth: true
                opacity: hasSelection && group.checkedButton !== profile.button ? 0.0 : 1.0
                ProfileSelector {
                    id: profileSelector
                    Layout.preferredHeight: flick.height - appSettings.height - profile.button.height - (Theme.spacing * 2)//flick.height - (Theme.spacing * 4)- appSettings.height - appSettings.height - about.height - footer.height
                    interactive: flick.height-appSettings.height < profileSelector.contentHeight
                    Layout.fillWidth: true
                }
                WalkerItem.description: qsTr("Select Profile")
                WalkerItem.weight: 601
                WalkerItem.onEnter: {
                    group.checkedButton = group.buttons[0]
                }

            }
            ExpansionPanel {
                id: appSettings
                text: qsTr("App Settings")
                Layout.fillWidth: true
                Layout.fillHeight: group.checkedButton === appSettings.button
                opacity: hasSelection && group.checkedButton !== appSettings.button ? 0.0 : 1.0
                WalkerItem.description: qsTr("Application Settings")
                WalkerItem.weight: 603
                WalkerItem.onEnter: {
                    group.checkedButton = group.buttons[1]
                }
                GridLayout {
                    columns: 2
                    rowSpacing: Theme.spacing
                    Layout.topMargin: Theme.margin
                    Layout.bottomMargin: Theme.margin
                    Layout.leftMargin: Theme.margin
                    Layout.alignment: Qt.AlignTop | Qt.AlignVCenter
                    Label {
                        Layout.leftMargin: Theme.margin
                        text: qsTr("Dark Mode:")
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    Switch {
                        id: darkModeSw
                        checked: Theme.darkMode
                        onClicked: {
                            Theme.darkMode = darkModeSw.checked
                        }
                        WalkerItem.description: qsTr("Set/Unset dark mode.")
                        WalkerItem.weight: 604
                        WalkerItem.onEnter: {
                            Theme.darkMode = !Theme.darkMode
                        }
                        WalkerItem.onExit: {
                            Theme.darkMode = !Theme.darkMode
                        }
                    }
                    Label {
                        Layout.leftMargin: Theme.margin
                        text: qsTr("Language:")
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    ComboBox {
                        id: langSelector
                        model: DiceMainController.langModel
                        Layout.fillWidth: true
                        currentValue: DiceMainController.lang ? DiceMainController.lang : Qt.uiLanguage
                        textRole: "display"
                        valueRole: "code"
                        hoverEnabled: true
                        onCurrentValueChanged: {
                            if(DiceMainController.lang !== langSelector.currentValue)
                                DiceMainController.lang = langSelector.currentValue
                        }
                        WalkerItem.description: qsTr("Change language.")
                        WalkerItem.weight: 605
                    }
                    Label {
                        Layout.leftMargin: Theme.margin
                        text: qsTr("Ui Tour:")
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Start")
                        onClicked: {
                            startUiTour()
                        }
                    }
                }
            }
            ExpansionPanel {
                id: about
                text: qsTr("About")
                Layout.fillWidth: true
                opacity: hasSelection && group.checkedButton !== about.button ? 0.0 : 1.0
                WalkerItem.description: qsTr("Version information.")
                WalkerItem.weight: 606
                WalkerItem.onEnter: {
                    group.checkedButton = group.buttons[2]
                }
                WalkerItem.onExit: {
                    group.checkState = Qt.Unchecked
                    group.previous = null
                }
                GridLayout {
                    columns: 2
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    Label {
                        text: qsTr("Version:")
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    Label {
                        text: DiceMainController.version
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    Label {
                        text: qsTr("Date:")
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    Label {
                        text: DiceMainController.dateVersion
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    Label {
                        text: qsTr("Sha1:")
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                    Label {
                        text: DiceMainController.hashVersion
                        font.pixelSize: Theme.finalDiceResultFontSize
                    }
                }
            }
        }
    }


    RowLayout {
        id: footer
        Layout.fillWidth: true
        //Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        ToolButton {
            id: add
            icon.source: "qrc:/assets/plus2.svg"
            icon.width: Theme.iconSize * 2
            icon.height: Theme.iconSize * 2
            icon.color: Theme.transparent
            visible: group.checkedButton === profile.button
            background: Item {}
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            flat: true
            WalkerItem.description: qsTr("Add Profile.")
            WalkerItem.weight: 602
            onClicked: {
                DiceMainController.settingsCtrl.sessions.addSession()
            }
        }
        Item {//filler
            Layout.fillWidth: true
        }

        ToolButton {
            id: link
            icon.source: "qrc:/assets/external.svg"
            icon.width: Theme.iconSize * 2
            icon.height: Theme.iconSize * 2
            icon.color: Theme.textColor
            background: Item {}
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            flat: true
            WalkerItem.description: qsTr("Documentation.")
            WalkerItem.weight: 607


            onClicked: Qt.openUrlExternally("https://invent.kde.org/rolisteam/rolisteam-diceparser/-/blob/master/HelpMe.md?ref_type=heads")
        }
    }
}
