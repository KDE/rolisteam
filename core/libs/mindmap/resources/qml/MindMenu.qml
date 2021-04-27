import QtQuick 2.12
import QtQuick.Controls 2.2

Menu {
    id: menu
    property QtObject ctrl
    MenuItem {
        text: qsTr("Add Root")
        onTriggered: ctrl.addBox("")
    }
    MenuSeparator { }
    /*MenuItem {
        text: qsTr("New")
        onTriggered: ctrl.resetData()
    }
    MenuItem {
        text: qsTr("Open File…")
        onTriggered: openDialog.open()
    }*/
    MenuItem {
        text: qsTr("Save")

        onTriggered: {
            if(ctrl.filename)
                ctrl.saveFile();
            else
                saveDialog.open()
        }
    }
    /*MenuItem {
        text: qsTr("Save As…")
        onTriggered: {
            saveDialog.open()
        }
    }*/
    MenuItem {
        text: qsTr("Remove Selection")
        enabled: ctrl.hasSelection
        onTriggered: {
            ctrl.removeSelection();
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Undo")
        enabled: ctrl.canUndo
        onTriggered: {
            ctrl.undo()
        }
    }
    MenuItem {
        text: qsTr("Redo")
        enabled: ctrl.canRedo
        onTriggered: {
            ctrl.redo()
        }
    }
    MenuSeparator { }
    /*MenuItem {
        text: qsTr("Import File…")
        onTriggered: {
            importDialog.open()
        }
    }*/
   // MenuSeparator { }
    MenuItem {
        text: qsTr("Automatic Spacing")
        checkable: true
        checked: ctrl.spacing
        onTriggered: ctrl.spacing = checked
    }
}
