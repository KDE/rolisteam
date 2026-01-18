import QtQuick
import QtQuick3D
import QtQuick3D.Physics
import QtQuick3D.Helpers
import QtQuick.Controls
import QtQuick.Layouts
import QtQml
import Controllers
import DicePhysics

ApplicationWindow {
    id: root
    x: Dice3DCtrl.x
    y: Dice3DCtrl.y
    width: Dice3DCtrl.width
    height: Dice3DCtrl.height

    onWidthChanged: {
        if (root.width !== Dice3DCtrl.width && Dice3DCtrl.normalDialogMode)
            Dice3DCtrl.size = Qt.size(root.width, root.height)
    }
    onHeightChanged: {
        if (root.height !== Dice3DCtrl.height && Dice3DCtrl.normalDialogMode)
            Dice3DCtrl.size = Qt.size(root.width, root.height)
    }

    visible: Dice3DCtrl.displayed

    flags: Dice3DCtrl.normalDialogMode ? Qt.Window : Qt.WA_TranslucentBackground | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint | Qt.Popup
    color:  Dice3DCtrl.normalDialogMode ? "#88FFFFFF" : "transparent"

    property real currDrawerWidth: menu.width * menu.position

    title: qsTr("Rolisteam 3D dice roller %1".arg(Dice3DCtrl.normalDialogMode))

    DicePlan {
        id: dicePlan
        anchors.fill: parent
        ctrl: Dice3DCtrl
        factor: menu.factor
        parentWidth: root.width
        parentHeight: root.height
        Action {
            id: selectAllAct
            text: qsTr("Select All dice")
            icon.source:"qrc:/dice3d/icons/selectall.svg"
            shortcut: StandardKey.SelectAll
            onTriggered: dicePlan.selectAll()
        }
        Action {
            id: resetSelectAct
            text: qsTr("Remove Selection")
            icon.source: "qrc:/dice3d/icons/removeSelect.svg"
            shortcut: "Ctrl+Shift+A"
            onTriggered: dicePlan.resetSelection()
        }
        Action {
            id: selectRectAct
            text: qsTr("Selection by zone")
            icon.source: "qrc:/dice3d/icons/selectRect.svg"
            shortcut: "Ctrl+r"
            onTriggered: dicePlan.startRectSelection()
        }
        Action {
            id: removeAllDice
            text: qsTr("Remove all dice")
            icon.source: "qrc:/dice3d/icons/deleteall.svg"
            icon.color: "transparent"
            onTriggered: Dice3DCtrl.cleanUp()
        }
        Action {
            id: resetSettings
            text: qsTr("Reset Settings")
            icon.source: "qrc:/dice3d/icons/resetSettings.svg"
            onTriggered: Dice3DCtrl.reset()
        }
        SideMenu {
            id: menu
            selectAll: selectAllAct
            resetSelection:resetSelectAct
            rectSelection: selectRectAct
            deleteAll: removeAllDice
            resetSettings: resetSettings
        }

        RoundButton {
            id: iconOpen
            icon.source: "qrc:/dice3d/icons/menuIcon.svg"
            x: root.currDrawerWidth
            onClicked: {
                menu.open()
            }
        }

        Label {
            id: selectionCount
            text: dicePlan.selectedCount
            visible: dicePlan.selectedCount > 0
            font.pixelSize: 30
            font.bold: true
            anchors.horizontalCenter: iconOpen.horizontalCenter
            anchors.top: iconOpen.bottom
            anchors.topMargin: 20
            horizontalAlignment: Label.AlignHCenter

            width: Math.max(implicitHeight, implicitWidth)
            height: width
            background: Rectangle {
                color: "white"
                radius: width/2
                opacity: 0.8
            }
        }
    }
}



//! [window]
