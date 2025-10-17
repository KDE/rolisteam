import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import treeDicer

Panel {
    id: root
    title: qsTr("Settings")

    ListView {
        id: view
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: DiceMainController.settingsCtrl.sessions
        spacing: Theme.spacing
        clip: true

        delegate: SwipeDelegate {
            width:view.width
            height: lyt.implicitHeight + 2 * Theme.padding
            property bool edition: false
            RowLayout {
                id: lyt
                anchors.margins: Theme.margin
                anchors.fill: parent
                TextField {
                    enabled: edition
                    text: model.name
                    onEditingFinished: model.name = text
                    font.pointSize: Theme.finalDiceResultFontSize
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    //anchors.fill: parent
                    Layout.fillWidth: true
                }
                ToolButton {
                    icon.source: edition ?  "qrc:/assets/check.svg" :  "qrc:/assets/edit.svg"
                    icon.width: Theme.iconSize * 2
                    icon.height: Theme.iconSize * 2
                    icon.color: "transparent"
                    background: Item {}
                    flat: true
                    opacity: swipe.position === 0 ? 1.0 : 0.0
                    enabled: swipe.position === 0
                    onClicked: {
                        edition = !edition
                    }
                }
            }
            background:  Rectangle {
                border.width: 1
                radius: Theme.radius
                color: DiceMainController.settingsCtrl.currentSessionIndex === model.index ? "#662e8b57" : "transparent"
            }

            onClicked: {
                DiceMainController.saveData()
                DiceMainController.settingsCtrl.currentSessionIndex = model.index
            }

            swipe.right: Label {
                id: deleteLabel
                text: qsTr("Delete")
                color: "white"
                verticalAlignment: Label.AlignVCenter
                padding: 12
                height: parent.height
                anchors.right: parent.right
                opacity: swipe.position === 0 ? 0.0 : 1.0

                SwipeDelegate.onClicked: DiceMainController.settingsCtrl.sessions.removeSession(index)

                background: Rectangle {
                    color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
                    radius: Theme.radius
                }
            }
        }
    }
    RowLayout {
        Layout.fillWidth: true
        ToolButton {
            id: add
            icon.source: "qrc:/assets/plus2.svg"
            icon.width: Theme.iconSize * 2
            icon.height: Theme.iconSize * 2
            icon.color: "transparent"
            background: Item {}
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            flat: true
            onClicked: {
                DiceMainController.settingsCtrl.sessions.addSession()
            }
        }
        Label {
            Layout.fillWidth: true
            text: qsTr("v%1 - %2 - sha1:%3").arg(DiceMainController.version).arg(DiceMainController.dateVersion).arg(DiceMainController.hashVersion)
            horizontalAlignment: Label.AlignHCenter
            verticalAlignment: Label.AlignBottom
        }

        ToolButton {
            id: link
            icon.source: "qrc:/assets/external.svg"
            icon.width: Theme.iconSize * 2
            icon.height: Theme.iconSize * 2
            icon.color: "transparent"
            background: Item {}
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            flat: true
            onClicked: Qt.openUrlExternally("https://invent.kde.org/rolisteam/rolisteam-diceparser/-/blob/master/HelpMe.md?ref_type=heads")
        }
    }
}
