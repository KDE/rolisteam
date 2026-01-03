import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import dicely
import Walker

Panel {
    id: root
    title: qsTr("Aliases")

    ListView {
        id: view
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: DiceMainController.aliases
        spacing: Theme.spacing
        clip: true
        property int editIndex: -1
        WalkerItem.weight: 402
        WalkerItem.description: qsTr("Alias List")

        delegate: SwipeDelegate {
            id: pane
            width:view.width
            height: pane.editMode ? edit.implicitHeight : lyt.implicitHeight + 2 * Theme.padding
            visible: view.editIndex === index || view.editIndex < 0
            property bool editMode: false


            RowLayout {
                id: lyt
                anchors.fill: parent
                visible: !pane.editMode
                Label {
                    id: label
                    text: qsTr("<b>%1</b><br/>%2 => %3<br/>Regex: %4").arg(model.comment ? model.comment : qsTr("Alias #%1").arg(index))
                                                                .arg(model.pattern ? model.pattern : qsTr("Pattern"))
                                                                .arg(model.command ? model.command : qsTr("Command"))
                                                                .arg(model.method ? qsTr("Off"): qsTr("On"))
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    opacity: 1 - Math.abs(swipe.position/2)
                    verticalAlignment: Label.AlignVCenter
                    padding: Theme.padding
                    //onTextEdited: model.comment = text
                }
                Image {
                    Layout.fillHeight: true
                    source: "qrc:/assets/edit.svg"
                    sourceSize.width: Theme.iconSize * 2
                    sourceSize.height: Theme.iconSize * 2
                    fillMode: Image.PreserveAspectFit
                    visible: swipe.position === 0
                    WalkerItem.weight: 401
                    WalkerItem.description: qsTr("Edit alias")
                    WalkerItem.onExit: {
                        pane.editMode = true
                    }
                }
            }
            background:  Rectangle {
                color: model.disable ? Theme.disabledColor : Theme.contentBackGroundColor
                border.width: Theme.penWidth
                border.color: Theme.borderColor
                radius: Theme.radius
            }

            onClicked: {
                if(pane.editMode)
                    return

                pane.editMode = true
                view.editIndex = index

            }

            ColumnLayout {
                id: edit
                visible: pane.editMode
                anchors.fill: parent
                anchors.margins: Theme.margin
                opacity: 1 - Math.abs(swipe.position)

                Label {
                    text: qsTr("Name:")
                    font.pointSize: Theme.textFieldFontSize
                }
                TextField {
                    id: nameField
                    text: model.comment
                    onTextEdited: model.comment = text
                    Layout.fillWidth: true
                    font.pointSize: Theme.textFieldFontSize
                    WalkerItem.weight: 402
                    WalkerItem.description: qsTr("Set alias name")
                    WalkerItem.onEnter: {
                        nameField.text="My Game"
                    }
                }
                Label {
                    text: qsTr("Pattern:")
                    font.pointSize: Theme.textFieldFontSize
                }
                TextField {
                    id: patternField
                    text: model.pattern
                    placeholderText: "K, (.*)C(.*)"
                    onTextEdited: model.pattern = text
                    Layout.fillWidth: true
                    font.pointSize: Theme.textFieldFontSize
                    WalkerItem.weight: 403
                    WalkerItem.description: qsTr("Pattern which triggers the replace.")
                    WalkerItem.onExit: {
                        patternField.text = "(.*)MG"
                    }
                }
                Label {
                    text: qsTr("Command:")
                    font.pointSize: Theme.textFieldFontSize
                }
                TextField {
                    id: cmdField
                    text: model.command
                    placeholderText: "d10e10k, \\1d10e10c[>=\\2]"
                    onTextEdited: model.command = text
                    Layout.fillWidth: true
                    font.pointSize: Theme.textFieldFontSize
                    WalkerItem.weight: 404
                    WalkerItem.description: qsTr("Real command")
                    WalkerItem.onExit: {
                        cmdField.text = "d10e10c[>7]"
                    }
                }
                Switch {
                    text: qsTr("Regular expression")
                    checked: !model.method
                    onToggled: model.method = checked
                    font.pointSize: Theme.textFieldFontSize
                }
                Switch {
                    id: disableBox
                    text: qsTr("Disable")
                    checked: model.disable
                    onToggled: model.disable = checked
                    font.pointSize: Theme.textFieldFontSize
                    WalkerItem.weight: 405
                    WalkerItem.description: qsTr("Disable")
                }
                ToolButton {
                    icon.source: "qrc:/assets/check.svg"
                    icon.width: Theme.iconSize * 2
                    icon.height: Theme.iconSize * 2
                    icon.color: Theme.transparent
                    text: qsTr("Save and back")
                    flat: true
                    //color: Theme.textColor
                    Layout.fillWidth: true
                    background: Rectangle {
                        border.width: Theme.penWidth
                        radius: Theme.radius
                        border.color: Theme.borderColor
                        color: Theme.placeHolderTextColor
                    }
                    WalkerItem.weight: 406
                    WalkerItem.description: qsTr("Save the alias")

                    onClicked: {
                        pane.editMode = false
                        view.editIndex = -1
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

                SwipeDelegate.onClicked: DiceMainController.aliases.deleteAt(index)

                WalkerItem.weight: 407
                WalkerItem.description: qsTr("Delete")
                WalkerItem.onEnter: {
                    swipe.position = 1.
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
        flat: true
        background: Item {}
        onClicked: {
            DiceMainController.addAlias()
        }
        WalkerItem.weight: 401
        WalkerItem.description: qsTr("Add alias")
        WalkerItem.onExit: {
            DiceMainController.addAlias()
        }
    }


}
