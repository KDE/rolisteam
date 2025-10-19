import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import dicely

Panel {
    id: root
    title: qsTr("Macros")

    ListView {
        id: view
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: DiceMainController.macros
        spacing: Theme.spacing
        clip: true

        delegate: SwipeDelegate {
            id: delegate
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
                    placeholderText: qsTr("Name")
                    font.pointSize: Theme.textFieldFontSize
                }
                TextField {
                    enabled: edition
                    Layout.fillWidth: true
                    text: model.command
                    onEditingFinished: model.command = text
                    placeholderText: qsTr("command")
                    font.pointSize: Theme.textFieldFontSize
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

            swipe.right: ToolButton {
                id: deleteLabel
                text: qsTr("Delete")
                 width: Math.max(deleteLabel.implicitWidth, height)
                icon.color: Theme.deleteTextColor
                icon.source: "qrc:/assets/trashbin.svg"
                icon.width: Theme.iconSize
                icon.height: Theme.iconSize
                display: ToolButton.TextUnderIcon
                padding: Theme.padding
                height: parent.height
                anchors.right: parent.right
                opacity: swipe.position === 0 ? 0.0 : 1.0

                SwipeDelegate.onClicked: DiceMainController.macros.removeMacro(index)

                background: Rectangle {
                    color: deleteLabel.SwipeDelegate.pressed ? Qt.darker(Theme.deleteBtnColor, 1.1) : Theme.deleteBtnColor
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
        icon.color: Theme.transparent
        background: Item {}
        flat: true
        onClicked: {
            DiceMainController.macros.addMacro()
        }
    }
}
