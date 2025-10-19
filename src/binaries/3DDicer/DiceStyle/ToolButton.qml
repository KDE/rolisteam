import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import dicely

T.ToolButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: Theme.padding
    spacing: Theme.spacing
    flat: true

    icon.width: Theme.iconSize
    icon.height: Theme.iconSize
    icon.color: visualFocus ? Theme.highLightColor : Theme.textColor

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.icon.color
    }

    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40

        opacity: control.down ? 1.0 : 0.5
        color: control.down || control.checked || control.highlighted ? Theme.midColor : Theme.transparent
        //color: control.down || control.checked || control.highlighted ? Theme.midColor : control.palette.button

        border.color: {
            if(control.flat)
                return Theme.transparent
            else if (control.visualFocus)
                return Theme.highLightColor
            else
                return Theme.borderColor
        }
        border.width: control.visualFocus ? Theme.penWidth * 2 : Theme.penWidth
    }
}
