import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.MenuItem {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 6
    spacing: 6

    icon.width: 24
    icon.height: 24
    icon.color: control.style.windowText

    contentItem: IconLabel {
        readonly property real arrowPadding: control.subMenu && control.arrow ? control.arrow.width + control.spacing : 0
        readonly property real indicatorPadding: control.checkable && control.indicator ? control.indicator.width + control.spacing : 0
        leftPadding: !control.mirrored ? indicatorPadding : arrowPadding
        rightPadding: control.mirrored ? indicatorPadding : arrowPadding

        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: Qt.AlignLeft
        opacity: control.enabled ? 1.0 : 0.4

        icon: control.icon
        text: control.text
        font: control.font
        color: control.style.windowText
    }

    indicator: ColorImage {
        x: control.mirrored ? control.width - width - control.rightPadding : control.leftPadding
        y: control.topPadding + (control.availableHeight - height) / 2

        visible: control.checked
        source: control.checkable ? "qrc:/qt-project.org/imports/QtQuick/Controls/Basic/images/check.png" : ""
        color: control.style.windowText
        defaultColor: "#353637"
    }

    arrow: ColorImage {
        x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2

        visible: control.subMenu
        mirror: control.mirrored
        source: control.subMenu ? "qrc:/qt-project.org/imports/QtQuick/Controls/Basic/images/arrow-indicator.png" : ""
        color: control.style.windowText
        defaultColor: "#353637"
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 40
        x: 1
        y: 1
        width: control.width - 2
        height: control.height - 2
        color: control.down ? control.style.midlight : control.highlighted ? control.style.light : "transparent"
        border.color: control.style.dark
        border.width: Qt.styleHints.accessibility.contrastPreference === Qt.HighContrast && control.highlighted ? 1 : 0
    }
}
