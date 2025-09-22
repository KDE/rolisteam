import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import treeDicer

Panel {
    id: root
    title: qsTr("Character Sheet")

    ListView {
        id: view
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: DiceMainController.propertiesModel
        spacing: Theme.spacing

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
                    Layout.fillWidth: true
                    text: model.key
                    onEditingFinished: model.key = text
                    placeholderText: qsTr("Key")
                    font.pointSize: Theme.textFieldFontSize
                }
                TextField {
                    enabled: edition
                    Layout.minimumWidth: Theme.minimalTextField
                    text: model.value
                    onEditingFinished: model.value = text
                    placeholderText: qsTr("Value")
                    font.pointSize: Theme.textFieldFontSize
                }
                ToolButton {
                    icon.source: edition ?  "qrc:/assets/check.svg" :  "qrc:/assets/edit.svg"
                    icon.width: Theme.iconSize * 2
                    icon.height: Theme.iconSize * 2
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
            DiceMainController.propertiesModel.addField()
        }
    }
}
