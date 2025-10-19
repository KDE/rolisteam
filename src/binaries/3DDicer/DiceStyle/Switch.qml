import QtQuick
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import QtQuick.Templates as T
import dicely

T.Switch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 8
    spacing: 8

    property color highLigthed: Theme.checkedSwitchColor
    property color uncheckedColor: Theme.uncheckedSwitchColor

    icon.width: 16
    icon.height: 16
    icon.color: checked ? control.highLigthed : control.uncheckedColor

    indicator: SwitchIndicator {
        x: control.text ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding) : control.leftPadding + (control.availableWidth - width) / 2
        y: control.topPadding + (control.availableHeight - height) / 2
        control: control
        color: control.checked ? control.highLigthed : control.uncheckedColor //Theme.highLightColor : Theme.placeHolderTextColor
        border.color: Theme.fullOpacity(control.checked ? control.highLigthed : control.uncheckedColor) //Theme.highLightColor : Theme.placeHolderTextColor
        Ripple {
            x: parent.handle.x + parent.handle.width / 2 - width / 2
            y: parent.handle.y + parent.handle.height / 2 - height / 2
            width: 28
            height: 28
            pressed: control.pressed
            active: enabled && (control.down || control.visualFocus || control.hovered)
            //color: control.checked ? control.Material.highlightedRippleColor : control.Material.rippleColor
            //color: control.checked ? Theme.highLightColor : Theme.placeHolderTextColor
            color: control.checked ? control.highLigthed : control.uncheckedColor
        }
    }

    contentItem: Text {
        leftPadding: control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0

        text: control.text
        font: control.font
        color: control.enabled ? Theme.textColor : Theme.placeHolderTextColor
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
}

