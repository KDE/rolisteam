import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import dicely

T.PageIndicator {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: Theme.padding
    spacing: Theme.spacing

    delegate: Rectangle {
        implicitWidth: 8
        implicitHeight: 8

        radius: width / 2
        color: Theme.textColor

        opacity: index === control.currentIndex ? 1.0 : pressed ? 0.7 : 0.45

        required property int index

        Behavior on opacity { OpacityAnimator { duration: 100 } }
    }

    contentItem: Row {
        spacing: control.spacing

        Repeater {
            model: control.count
            delegate: control.delegate
        }
    }
}

