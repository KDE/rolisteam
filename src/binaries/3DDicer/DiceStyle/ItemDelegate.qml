import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import dicely

T.ItemDelegate {
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

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: control.display === IconLabel.IconOnly || control.display === IconLabel.TextUnderIcon ? Qt.AlignCenter : Qt.AlignLeft

        icon: control.icon
        text: control.text
        font: control.font
        //color: control.highlighted ? control.palette.highlightedText : control.palette.text
        color: Theme.textColor
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        visible: true
        opacity: control.pressed ? 0.8 : 1.0
        color: Theme.contentBackGroundColor
        border.width: Theme.penWidth
        border.color: Theme.borderColor
        radius: Theme.radius
        /*color: Color.blend(control.down ? control.palette.midlight : control.palette.light,
                                          control.palette.highlight, control.visualFocus ? 0.15 : 0.0)
        border.width: Qt.styleHints.accessibility.contrastPreference === Qt.HighContrast ? 1 : 0
        border.color: control.highlighted ? control.palette.highlight : control.palette.text*/
    }
}

