import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import dicely

ListView {
    id: view

    model: DiceMainController.settingsCtrl.sessions
    spacing: Theme.spacing
    clip: true
    interactive: false

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
                icon.color: Theme.transparent
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
            border.color: Theme.borderColor
            radius: Theme.radius
            color: DiceMainController.settingsCtrl.currentSessionIndex === model.index ? Theme.selectedColor : Theme.contentBackGroundColor
        }

        onClicked: {
            DiceMainController.saveData()
            DiceMainController.settingsCtrl.currentSessionIndex = model.index
        }

        swipe.right: ToolButton {
            id: deleteLabel

            height: parent.height
            property bool isOnlyOne: view.count === 1
            anchors.right: parent.right
            width: Math.max(deleteLabel.implicitWidth, height)
            opacity: swipe.position === 0 ? 0.0 : 1.0
            text: isOnlyOne ? qsTr("Last Profile") : qsTr("Delete")
            icon.source: isOnlyOne ? "qrc:/assets/lock.svg" : "qrc:/assets/trashbin.svg"
            icon.width: Theme.iconSize
            icon.height: Theme.iconSize
            display: ToolButton.TextUnderIcon
            icon.color: Theme.deleteTextColor

            SwipeDelegate.onClicked: {
                if(!isOnlyOne)
                    DiceMainController.settingsCtrl.sessions.removeSession(index)
            }
            background: Rectangle {
                color: deleteLabel.SwipeDelegate.pressed ? Qt.darker(Theme.deleteBtnColor, 1.1) : Theme.deleteBtnColor
                radius: Theme.radius
            }
        }
    }
}
