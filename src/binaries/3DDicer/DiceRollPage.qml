import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import treeDicer

Panel {
    id: root
    title: qsTr("Dice Rolls")

    ListView {
        id: view
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: DiceMainController.model
        spacing: 5
        clip: true

        delegate: ItemDelegate {
            required property string result
            required property string command
            required property string comment
            required property string time
            required property string details
            width:view.width
            height: lyt.implicitHeight + padding
            ColumnLayout {
                id: lyt
                anchors.fill: parent
                anchors.rightMargin: Theme.margin
                anchors.leftMargin: Theme.margin
                RowLayout {
                    Layout.fillWidth: true
                    ToolButton {
                        id: detailsBtn
                        icon.source: "qrc:/assets/plus.svg"
                        icon.width: Theme.iconSize
                        icon.height:Theme.iconSize
                        icon.color: "transparent"
                        display: AbstractButton.IconOnly
                        checkable: true
                        flat: true
                        background: Item{}
                        rotation: checked ? 45 : 0
                    }
                    Label {
                        text: result
                        Layout.fillWidth: true
                        font.pointSize: Theme.finalDiceResultFontSize
                        font.weight: Font.Bold
                        horizontalAlignment: Label.AlignHCenter
                    }
                    ColumnLayout{
                        ToolButton {
                            id: rerolls
                            icon.source: "qrc:/assets/redo.svg"
                            icon.width: Theme.iconSize
                            icon.height:Theme.iconSize
                            icon.color: "transparent"
                            display: AbstractButton.IconOnly
                            onClicked: DiceMainController.runCommand(command)
                        }
                        Label {
                            text: time
                            font.pointSize: Theme.timeFontSize

                            font.weight: Font.Light
                            horizontalAlignment: Label.AlignRight
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    visible: detailsBtn.checked
                    spacing: 0
                    Label {
                        text: "%1".arg(command)
                        textFormat: Text.PlainText
                    }
                    Label {
                        text: ": %1".arg(details)
                        textFormat: Text.RichText
                    }
                }

            }

            onPressAndHold: {
                cmdField.text = command
            }

            background: Rectangle {
                radius: Theme.radius
                border.width: 1
                border.color: "black"
            }
        }
    }
    GroupBox {
        id: groupBox
        Layout.fillWidth: true
        Layout.preferredHeight: root.height * 0.33
        clip: true
        ColumnLayout
        {
            anchors.fill: parent
            spacing: Theme.spacing
            SwipeView {
                id: stackLayout
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: DiceMainController.currentPanel
                onCurrentIndexChanged: DiceMainController.currentPanel = currentIndex
                interactive: gridLyt.count > 0
                ColumnLayout {

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    RowLayout {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true
                        TextField {
                            id: cmdField
                            placeholderText: qsTr("Type your command e.g: 3d10…")
                            Layout.fillWidth: true


                            font.pointSize: Theme.commandFontSize
                            onEditingFinished: {
                                DiceMainController.runCommand(cmdField.text)
                                cmdField.text = ""
                            }
                        }
                        ToolButton {
                            text: qsTr("Run")
                            enabled: cmdField.text.length > 0
                            icon.source: "qrc:/assets/send.svg"
                            icon.width: Theme.iconSize * 2
                            icon.height:Theme.iconSize * 2
                            icon.color: "transparent"
                            //Layout.fillHeight: true
                            display: AbstractButton.IconOnly
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            opacity: enabled ? 1.0 : 0.2
                            scale: pressed ? 0.8 : 1.0
                            background: Item{}
                            onClicked: {
                                DiceMainController.runCommand(cmdField.text)
                                cmdField.text = ""
                            }
                        }
                    }
                    /*Item {
                        id: margin
                        Layout.columnSpan: 2
                        Layout.preferredHeight: 40
                    }*/
                }

                GridView {
                    id: gridLyt
                    visible: stackLayout.currentIndex === 1
                    //anchors.fill: parent

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellHeight: cellWidth + Theme.spacing/2
                    cellWidth: (gridLyt.width  - (3 * Theme.spacing)) / Math.max(4 ,Math.min(4, gridLyt.count)) + Theme.spacing/2

                    model: DiceMainController.macros
                    delegate: ItemDelegate {
                        background: Rectangle {
                            border.width: 1
                        }
                        width: gridLyt.cellWidth - Theme.spacing
                        height: gridLyt.cellHeight - Theme.spacing
                        Label {
                            anchors.centerIn: parent
                            text: model.name
                            font.pointSize: Theme.commandFontSize
                        }
                        scale: pressed ? 0.8 : 1.0
                        onClicked: {
                            DiceMainController.runCommand(model.command)
                        }
                    }

                }
            }
            PageIndicator {
                currentIndex: stackLayout.currentIndex
                count: stackLayout.count
                Layout.alignment: Qt.AlignCenter
            }
        }
    }
}
