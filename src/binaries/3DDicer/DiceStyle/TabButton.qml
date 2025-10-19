import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import dicely

T.TabButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: Theme.padding
    spacing: Theme.spacing

    icon.width: Theme.iconSize
    icon.height: Theme.iconSize
    icon.color: checked ? Theme.windowColor : Theme.textColor

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.checked ? Theme.windowColor : Theme.textColor
    }

    background: Rectangle {
        implicitHeight: 40
        color: Color.blend(control.checked ? Theme.windowColor : Theme.darkColor,
                                             Theme.midColor, control.down ? 0.5 : 0.0)
        border.width: 0
        border.color: Theme.windowColor
    }
}
