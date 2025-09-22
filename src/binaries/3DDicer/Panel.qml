import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import treeDicer
Pane {
    id: root

    property alias title: titleLabel.text
    default property alias children: mainLyt.children

    ColumnLayout {
        id: mainLyt
        anchors.fill: parent
        spacing: Theme.spacing

        Label {
            id: titleLabel
            font.bold: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            horizontalAlignment: Label.AlignHCenter
            font.pointSize: Theme.titleFontSize
            /*Rectangle {
                anchors.fill: parent
                color: "red"
                opacity: 0.2
            }*/
        }
    }
}
