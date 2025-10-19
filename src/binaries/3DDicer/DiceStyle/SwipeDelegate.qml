import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import dicely

T.SwipeDelegate {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: Theme.padding
    spacing: Theme.spacing

    icon.width: Theme.iconSize
    icon.height: Theme.iconSize
    icon.color: Theme.textColor

    swipe.transition: Transition { SmoothedAnimation { velocity: 3; easing.type: Easing.InOutCubic } }

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: control.display === IconLabel.IconOnly || control.display === IconLabel.TextUnderIcon ? Qt.AlignCenter : Qt.AlignLeft

        icon: control.icon
        text: control.text
        font: control.font
        color: Theme.textColor
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        color: Theme.contentBackGroundColor
        //color: Color.blend(control.down ? control.palette.midlight : control.palette.light,
//                                          control.palette.highlight, control.visualFocus ? 0.15 : 0.0)
        // The condition of (control.down || control.highlighted || control.visualFocus)
        // came from the ItemDelegate.qml
        border.width: Theme.penWidth
        border.color: Theme.borderColor
        radius: Theme.radius
        /*border.width: Qt.styleHints.accessibility.contrastPreference === Qt.HighContrast &&
                      (control.down || control.highlighted || control.visualFocus) ? 1 : 0
        border.color: control.down || control.highlighted || control.visualFocus ?
                          control.palette.highlightedText : control.palette.text*/
    }
}

