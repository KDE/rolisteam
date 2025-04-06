import QtQuick 2.0
import QtQuick.Controls 2.15
import Customization 1.0

TextField {
    id: _root
    objectName: "RequiredTextField"
    property QtObject style: Theme.styleSheet("ValidField")
    property bool validInput

    background: Rectangle {
        border.color: !_root.enabled ? "darkgray" : _root.activeFocus ? _root.style.inputColor : _root.validInput ? _root.style.validColor : _root.style.invalidColor
        border.width: _root.validInput ? 1 : 3
        implicitWidth: 100
        implicitHeight: 20
    }
}
