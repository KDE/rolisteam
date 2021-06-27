import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T
import Customization 1.0

T.Pane {
    id: control

    property QtObject style: Theme.styleSheet("Controls")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                                contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                                 contentHeight + topPadding + bottomPadding)

    padding: 12

    background: Rectangle {
        color: control.style.backgroundColor
    }

}
