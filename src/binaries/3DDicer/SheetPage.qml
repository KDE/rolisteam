import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import dicely
import Walker

Panel {
    id: root
    title: qsTr("Character Sheet")

    ListView {
        id: view
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: DiceMainController.propertiesModel
        spacing: Theme.spacing
        clip: true

        delegate: SwipeDelegate {
            id: delegate
            width:view.width
            height: lyt.implicitHeight + 2 * Theme.padding
            property bool edition: false
            WalkerItem.description: qsTr("Sheet Item")
            WalkerItem.weight: 502
            WalkerItem.onExit: {
                edition = !edition
            }
            RowLayout {
                id: lyt
                anchors.margins: Theme.margin
                anchors.fill: parent
                TextField {
                    id: keyField
                    enabled: edition
                    Layout.fillWidth: true
                    text: model.key
                    onEditingFinished: model.key = text
                    placeholderText: qsTr("Key")
                    font.pointSize: Theme.textFieldFontSize
                    WalkerItem.description: qsTr("Edit sheet Item name")
                    WalkerItem.weight: 503
                    WalkerItem.onEnter: {
                        edition = !edition
                        keyField.text = qsTr("Intelligence")
                    }
                }
                TextField {
                    id: valueField
                    enabled: edition
                    Layout.minimumWidth: Theme.minimalTextField
                    text: model.value
                    onEditingFinished: model.value = text
                    placeholderText: qsTr("Value")
                    font.pointSize: Theme.textFieldFontSize
                    WalkerItem.description: qsTr("Edit sheet Item name")
                    WalkerItem.weight: 504
                    WalkerItem.onEnter: {
                        valueField.text = "5"
                    }
                }
                ToolButton {
                    id: saveBtn
                    icon.source: edition ?  "qrc:/assets/check.svg" :  "qrc:/assets/edit.svg"
                    icon.width: Theme.iconSize * 2
                    icon.height: Theme.iconSize * 2
                    icon.color: Theme.transparent
                    background: Item {}
                    flat: true
                    opacity: swipe.position === 0 ? 1.0 : 0.0
                    enabled: swipe.position === 0
                    WalkerItem.description: qsTr("Save Sheet item")
                    WalkerItem.weight: 505
                    WalkerItem.onExit: {
                        edition = !edition
                    }
                    onClicked: {
                        edition = !edition
                    }
                }
            }

            swipe.right: ToolButton {
                id: deleteLabel
                text: qsTr("Delete")
                icon.color: Theme.deleteTextColor
                icon.source: "qrc:/assets/trashbin.svg"
                icon.width: Theme.iconSize
                icon.height: Theme.iconSize
                 width: Math.max(deleteLabel.implicitWidth, height)
                display: ToolButton.TextUnderIcon
                padding: Theme.padding
                height: parent.height
                anchors.right: parent.right
                opacity: swipe.position === 0 ? 0.0 : 1.0

                SwipeDelegate.onClicked: DiceMainController.propertiesModel.removeField(index)
                WalkerItem.description: qsTr("Save Sheet item")
                WalkerItem.weight: 506
                WalkerItem.onEnter: {
                    swipe.position = 1.0
                }

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
        WalkerItem.description: qsTr("Add sheet item")
        WalkerItem.weight: 501
        WalkerItem.onEnter: {
            DiceMainController.propertiesModel.addField()
        }
        onClicked: {
            DiceMainController.propertiesModel.addField()
        }
    }
}
