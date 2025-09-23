import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import QtQml
import treeDicer
import DicePhysics

Panel {
    id: root
    title: qsTr("3D Dice")

    property QtObject ctrl: DiceMainController.dice3dCtrl

    data: [ColorDialog {
        id: colorDialog
        property int side: 0
        parentWindow: Overlay.overlay
        onAccepted: {
            if( side == 4)
                root.ctrl.fourColor = selectedColor
            else if(side == 6 )
                root.ctrl.sixColor = selectedColor
            else if(side == 8 )
                root.ctrl.eightColor = selectedColor
            else if(side == 10 )
                root.ctrl.tenColor = selectedColor
            else if(side == 12 )
                root.ctrl.twelveColor = selectedColor
            else if(side == 20 )
                root.ctrl.twentyColor = selectedColor
            else if(side == 100 )
                root.ctrl.oneHundredColor = selectedColor
        }
    }
    ]

    Item {
        id: central
        Layout.fillHeight: true
        Layout.fillWidth: true

        Flickable {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            contentHeight: _toolBar.implicitHeight
            height: _toolBar.implicitHeight
            flickableDirection: Flickable.AutoFlickIfNeeded
            ScrollBar.vertical: ScrollBar {
                policy: Screen.height < 600 ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            }
           /* Rectangle {

                anchors.fill: parent
                color: "red"
                opacity: 0.23
                Timer {
                    running: true
                    repeat: true
                    onTriggered: console.log("Item:",parent.width," ",parent.height)
                }
            }*/
            ColumnLayout {
                id: _toolBar
                anchors.horizontalCenter: parent.horizontalCenter


                property int fourCount: 0
                property int sixCount: 0
                property int eightCount: 0
                property int tenCount: 0
                property int twelveCount: 0
                property int twentyCount: 0
                property int oneHundredCount: 0

                Label {
                    text: qsTr("Dice Size:")
                }

                Slider {
                    id: _factor
                    from: 15.0
                    to: 100.0
                    value: root.ctrl.factor
                    Layout.fillWidth: true

                    onValueChanged:  {
                        root.ctrl.factor = _factor.value;
                        console.log("value:",_factor.value)
                    }
                }

                CheckBox {
                    id: mouseCtrl
                    text: qsTr("disable mouseArea")
                    checked: false
                }

                ListModel {
                    id: colors
                    ListElement {
                        side: 4
                        type: DiceController.FOURSIDE
                    }
                    ListElement {
                        side: 6
                        type: DiceController.SIXSIDE
                    }
                    ListElement {
                        side: 8
                        type: DiceController.OCTOSIDE
                    }
                    ListElement {
                        side: 10
                        type: DiceController.TENSIDE
                    }
                    ListElement {
                        side: 12
                        type: DiceController.TWELVESIDE
                    }
                    ListElement {
                        side: 20
                        type: DiceController.TWENTYSIDE
                    }
                    ListElement {
                        side: 100
                        type: DiceController.ONEHUNDREDSIDE
                    }
                }

                Repeater {
                    model: colors

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "D%1".arg(model.side)
                            Layout.fillWidth: true
                        }

                        ToolButton {
                            icon.name: "list-remove"
                            icon.color: "transparent"
                            onClicked:  {
                                root.ctrl.removeDice(model.type)
                            }
                        }


                        Label {
                            id: label
                            property int value: root.ctrl.diceCount(model.type)
                            text: value
                            Connections {
                                target: root.ctrl
                                function onCountChanged() {
                                    label.value = root.ctrl.diceCount(model.type)
                                }
                            }
                        }

                        ToolButton {
                            icon.name: "list-add"
                            icon.color: "transparent"
                            onClicked:{
                                root.ctrl.addDice(model.type)
                            }
                        }

                        ToolButton {
                            Layout.preferredWidth: height
                            Layout.fillHeight: true
                            contentItem: Rectangle {
                                id: rect
                                color: root.ctrl.diceColor(model.type)

                                Connections {
                                    target: root.ctrl
                                    function onColorChanged() {
                                        rect.color = root.ctrl.diceColor(model.type)
                                    }
                                }
                            }
                            onClicked: {
                                colorDialog.selectedColor = root.ctrl.diceColor(model.type)
                                colorDialog.side = model.side
                                colorDialog.open()
                            }
                        }
                    }
                }

                RowLayout {
                    Label {
                        text: qsTr("Collision sound:")
                    }

                    ToolButton {
                        id: mute
                        checkable: true
                        checked: root.ctrl.muted
                        icon.name: checked ? "audio-volume-muted" : "audio-volume-high"
                    }
                }
                /*RowLayout {
                    Label {
                        text: qsTr("Dice Command:")
                    }
                    Label {
                        text: root.ctrl.dicePart
                    }

                    TextField {
                        id: command
                        onTextEdited: {
                            root.ctrl.diceCommand = text
                        }
                    }
                }*/
            }
        }

        DicePlan {
            id: diceGround
            anchors.fill: parent
            ctrl: DiceMainController.dice3dCtrl
            factor: root.ctrl.factor
            parentWidth: width
            parentHeight: height

            onWidthChanged: DiceMainController.dice3dSize = Qt.size(width, height)
            onHeightChanged: DiceMainController.dice3dSize = Qt.size(width, height)
            Component.onCompleted: DiceMainController.dice3dSize = Qt.size(width, height)
        }
    }
}
