import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import QtQml
import dicely
import DicePhysics

Panel {
    id: root
    title: qsTr("3D Dice")

    property QtObject ctrl: DiceMainController.dice3dCtrl
    titleVisible: false

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

        Label {
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Label.AlignHCenter
            font.pointSize: Theme.titleFontSize
            text: root.title
        }

        Flickable {
            id: ground
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            contentHeight: _toolBar.implicitHeight
            height: _toolBar.implicitHeight
            flickableDirection: Flickable.AutoFlickIfNeeded
            ScrollBar.vertical: ScrollBar {
                policy: Screen.height < 600 ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            }
            ColumnLayout {
                id: _toolBar
                anchors.horizontalCenter: parent.horizontalCenter
                width: ground.width


                property int fourCount: 0
                property int sixCount: 0
                property int eightCount: 0
                property int tenCount: 0
                property int twelveCount: 0
                property int twentyCount: 0
                property int oneHundredCount: 0

                Label {
                    text: qsTr("Dice Size:")
                    visible: DiceMainController.show3dMenu
                }

                Slider {
                    id: _factor
                    from: 15.0
                    to: 100.0
                    value: root.ctrl.factor
                    Layout.fillWidth: true
                    visible: DiceMainController.show3dMenu

                    onValueChanged:  {
                        root.ctrl.factor = _factor.value;
                    }
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
                        visible: DiceMainController.show3dMenu
                        Label {
                            text: "D%1".arg(model.side)
                            Layout.fillWidth: true
                            font.pointSize: Theme.textFieldFontSize
                        }

                        ToolButton {
                            icon.source: "qrc:/assets/remove.svg"
                            icon.color: Theme.transparent
                            icon.width: Theme.iconSize
                            icon.height: Theme.iconSize
                            onClicked:  {
                                root.ctrl.removeDice(model.type)
                            }
                        }


                        Label {
                            id: label
                            property int value: root.ctrl.diceCount(model.type)
                            text: value
                            font.pointSize: Theme.textFieldFontSize
                            Connections {
                                target: root.ctrl
                                function onCountChanged() {
                                    label.value = root.ctrl.diceCount(model.type)
                                }
                            }
                        }

                        ToolButton {
                            icon.source: "qrc:/assets/plus3.svg"
                            icon.color: Theme.transparent
                            icon.width: Theme.iconSize
                            icon.height: Theme.iconSize
                            onClicked:{
                                root.ctrl.addDice(model.type)
                            }
                        }

                        ToolButton {
                            Layout.preferredWidth: height
                            Layout.fillHeight: true
                            Layout.minimumHeight: Theme.colorButtonSize
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
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                    ToolButton {
                        //select all
                        icon.source: "qrc:/assets/selectall.svg"
                        icon.color: Theme.textColor
                        icon.width: Theme.iconSize
                        icon.height: Theme.iconSize
                        visible: DiceMainController.show3dMenu
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        onClicked: loader.item.selectAll()
                    }
                    ToolButton {
                        //selection rect
                        icon.source: "qrc:/assets/selectRect.svg"
                        icon.color: Theme.textColor
                        icon.width: Theme.iconSize
                        icon.height: Theme.iconSize
                        checkable: true
                        visible: DiceMainController.show3dMenu
                        checked: loader.item.rectSelect
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        onClicked: loader.item.rectSelect = true
                    }
                    ToolButton {
                        // hide menu
                        icon.source: "qrc:/assets/menu.svg"
                        icon.color: Theme.textColor
                        icon.width: Theme.iconSize
                        icon.height: Theme.iconSize
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        onClicked: {
                            DiceMainController.show3dMenu = !DiceMainController.show3dMenu
                        }
                    }
                }
            }
        }

        Component {
            id: diceComp
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

        Loader {
            id: loader
            sourceComponent: diceComp
            asynchronous: true
            anchors.fill: parent
        }
    }
}
