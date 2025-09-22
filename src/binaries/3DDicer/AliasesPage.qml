import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import treeDicer

Panel {
    id: root
    title: qsTr("Aliases")

    ListView {
        id: view
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: DiceMainController.aliases
        spacing: 5
        property int editIndex: -1

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
                }
            }
            background:  Rectangle {
                color: model.disable ? "#66DC143C" : "transparent"
                border.width: 1
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
                    text: model.comment
                    onTextEdited: model.comment = text
                    Layout.fillWidth: true
                    font.pointSize: Theme.textFieldFontSize
                }
                Label {
                    text: qsTr("Pattern:")
                    font.pointSize: Theme.textFieldFontSize
                }
                TextField {
                    text: model.pattern
                    placeholderText: "K, (.*)C(.*)"
                    onTextEdited: model.pattern = text
                    Layout.fillWidth: true
                    font.pointSize: Theme.textFieldFontSize
                }
                Label {
                    text: qsTr("Command:")
                    font.pointSize: Theme.textFieldFontSize
                }
                TextField {
                    text: model.command
                    placeholderText: "d10e10k, \\1d10e10c[>=\\2]"
                    onTextEdited: model.command = text
                    Layout.fillWidth: true
                    font.pointSize: Theme.textFieldFontSize
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
                }
                ToolButton {
                    icon.source: "qrc:/assets/check.svg"
                    icon.width: Theme.iconSize * 2
                    icon.height: Theme.iconSize * 2
                    text: qsTr("Save and back")
                    flat: true
                    icon.color: "transparent"
                    Layout.fillWidth: true
                    background: Rectangle {
                        border.width: 1
                        radius: Theme.radius
                    }

                    onClicked: {
                        pane.editMode = false
                        view.editIndex = -1
                    }
                }
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

                SwipeDelegate.onClicked: DiceMainController.aliases.deleteAt(index)

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
        flat: true
        background: Item {}
        onClicked: {
            DiceMainController.addAlias()
        }
    }


}
