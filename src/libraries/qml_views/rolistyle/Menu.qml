import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.Menu {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    margins: 0
    overlap: 1

    delegate: MenuItem { }

    contentItem: ListView {
        implicitHeight: contentHeight
        model: control.contentModel
        interactive: Window.window
                     ? contentHeight + control.topPadding + control.bottomPadding > control.height
                     : false
        clip: true
        currentIndex: control.currentIndex

        ScrollIndicator.vertical: ScrollIndicator {}
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 40
        color: control.style.window
        border.color: control.palette.dark
    }

    T.Overlay.modal: Rectangle {
        color: Color.transparent(control.style.shadow, 0.5)
    }

    T.Overlay.modeless: Rectangle {
        color: Color.transparent(control.style.shadow, 0.12)
    }
}

