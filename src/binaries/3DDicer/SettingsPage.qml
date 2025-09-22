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

        delegate: SwipeDelegate {
            width:view.width
            height: lyt.implicitHeight + 2 * Theme.padding
            property bool edition: false
            Label {
                text: model.name
                font.pointSize: Theme.finalDiceResultFontSize
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                anchors.fill: parent
            }

            background:  Rectangle {
                border.width: 1
                radius: Theme.radius
                color: DiceMainController.settingsCtrl.currentSessionIndex === model.index ? "#662e8b57" : "transparent"
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

                SwipeDelegate.onClicked: DiceMainController.propertiesModel.removeField(index)

                background: Rectangle {
                    color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
                    radius: Theme.radius
                }
            }
        }
    }
    ToolButton {
        id: add
        icon.source: "qrc:/assets/plus2.svg"
        icon.width: Theme.iconSize * 2
        icon.height: Theme.iconSize * 2
        background: Item {}
        flat: true
        onClicked: {
            DiceMainController.settingsCtrl.sessions.addSession()
        }
    }
}
