import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import dicely

ColumnLayout {
    id: root
    property alias text: btn.text
    default property alias children: lyt.children
    property Button button: btn
    signal clicked
    width: root.width

    function setVisible(visible, begin) {
        if(visible && begin)
            root.visible = visible
        else if(!visible && !begin)
            root.visible = visible
    }

    Button {
        id: btn
        checkable: true
        Layout.fillWidth: true
        Layout.preferredHeight: implicitHeight
        height: implicitHeight
        onClicked: root.clicked()
    }
    ColumnLayout {
        id: lyt
        visible: btn.checked
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
    }



    Behavior on opacity {
        id: behavior
        SequentialAnimation{
           /* ScriptAction {
               script: root.setVisible((behavior.targetValue === 1), true)
            }*/
            NumberAnimation {
                duration: Theme.animationTime
            }
           /* ScriptAction {
               script: root.setVisible((behavior.targetValue === 1), false)
            }*/
        }

    }
}



