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

        delegate: Pane {
            required property string result
            required property string command
            required property string comment
            required property string time
            required property string details
            width:view.width
            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.fillWidth: true
                    ToolButton {
                        id: detailsBtn
                        icon.source: "qrc:/assets/plus.svg"
                        icon.width: Theme.iconSize
                        icon.height:Theme.iconSize
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
                Label {
                    text: "%1 : %2".arg(command).arg(details)
                    visible: detailsBtn.checked
                    textFormat: Text.StyledText
                    Layout.fillWidth: true
                }
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
        clip: true
        //Layout.maximumHeight: stackLayout.currentIndex === 0 ? Theme.iconSize*2 : root.height/2
        ColumnLayout
        {
            anchors.fill: parent
            spacing: Theme.spacing
            SwipeView {
                id: stackLayout
                Layout.fillWidth: true
                Layout.fillHeight: true
                GridLayout {
                    columns: 2
                    /*Layout.fillWidth: true
                    Layout.maximumHeight: Theme.iconSize*2
                    Layout.preferredHeight: Theme.iconSize
                    Layout.bottomMargin: Theme.margin*/
                    TextField {
                        id: cmdField
                        placeholderText: qsTr("Type your command e.g: 3d10…")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
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
                        Layout.fillHeight: true
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
                    Item {
                        id: margin
                        Layout.columnSpan: 2
                        Layout.preferredHeight: 40
                    }

                }

                GridLayout {
                    id: gridLyt
                    visible: stackLayout.currentIndex === 1
                    Layout.fillWidth: true
                    uniformCellHeights: true
                    uniformCellWidths: true
                    columns: 4
                    columnSpacing: Theme.spacing
                    Repeater {
                        model: 20
                        Label {

                            //width: (groupBox.width-(Theme.spacing*3)) / 4
                            background: Rectangle {
                                border.width: 1
                            }
                            text: "%1 - %2".arg(width).arg(height)

                            Layout.preferredHeight: width
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
