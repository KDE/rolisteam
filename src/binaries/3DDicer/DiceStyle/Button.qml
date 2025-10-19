import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import dicely

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: Theme.padding
    horizontalPadding: padding + 2
    spacing: Theme.spacing

    icon.width: Theme.iconSize
    icon.height: Theme.iconSize
    icon.color: control.checked || control.highlighted ? control.palette.brightText :
                control.flat && !control.down ? (control.visualFocus ? control.palette.highlight : control.palette.windowText) : control.palette.buttonText

    font.pointSize: Theme.textFieldFontSize

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.enabled ? Theme.textColor : Theme.placeHolderTextColor
        /*color: control.checked || control.highlighted ? control.palette.brightText :
               control.flat && !control.down ? (control.visualFocus ? control.palette.highlight : control.palette.windowText) : control.palette.buttonText*/
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        radius: Theme.radius
        visible: !control.flat || control.down || control.checked || control.highlighted
        color: Color.blend(control.checked || control.highlighted ? Theme.darkColor : Theme.contentBackGroundColor,
                                                                    control.palette.mid, control.down ? 0.5 : 0.0)
        border.color: control.visualFocus ? Theme.highLightColor : Theme.borderColor
        border.width: control.visualFocus ? Theme.penWidth * 2 : Theme.penWidth
    }
}

