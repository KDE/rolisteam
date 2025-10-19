pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as C
import QtQuick.Controls.impl
import QtQuick.Templates as T
import dicely

T.ComboBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    leftPadding: padding + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding: padding + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)

    delegate: ItemDelegate {
        required property var model
        required property int index
        padding: Theme.padding * 2

        width: ListView.view.width
        text: model[control.textRole]
        palette.text: hovered ? Theme.deleteTextColor : Theme.textColor
        palette.highlightedText: Theme.deleteTextColor
        font.weight: control.currentIndex === index ? Font.DemiBold : Font.Normal
        font.pixelSize: Theme.commandFontSize
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled

        background: Rectangle {
            border.width: Theme.penWidth
            border.color: Theme.borderColor
            color: Theme.contentBackGroundColor
        }
    }

    indicator: ColorImage {
        x: control.mirrored ? control.padding : control.width - width - control.padding
        y: control.topPadding + (control.availableHeight - height) / 2
        color: Theme.textColor
        defaultColor: Theme.textColor
        source: "qrc:/qt-project.org/imports/QtQuick/Controls/Basic/images/double-arrow.png"
        opacity: enabled ? 1 : 0.3
    }

    contentItem: T.TextField {
        leftPadding: !control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
        rightPadding: control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
        topPadding: 6 - control.padding
        bottomPadding: 6 - control.padding

        text: control.editable ? control.editText : control.displayText
        font.pixelSize: Theme.commandFontSize
        enabled: control.editable
        autoScroll: control.editable
        readOnly: control.down
        inputMethodHints: control.inputMethodHints
        validator: control.validator
        selectByMouse: control.selectTextByMouse

        color: control.editable ? Theme.textColor : Theme.textColor
        selectionColor: Theme.highLightColor
        selectedTextColor: Theme.deleteTextColor
        verticalAlignment: Text.AlignVCenter

        background: Rectangle {
            visible: control.enabled && control.editable && !control.flat
            border.width: parent && parent.activeFocus ? 2 : 1
            border.color: parent && parent.activeFocus ? Theme.highLightColor : Theme.contentBackGroundColor
            color: Theme.windowColor
        }
    }

    background: Rectangle {
        implicitWidth: 140
        implicitHeight: 40

        color: control.down ? Theme.darkColor : Theme.contentBackGroundColor
        border.color: !control.editable && control.visualFocus ? Theme.highLightColor : Theme.borderColor
        border.width: (!control.editable && control.visualFocus) ? Theme.penWidth * 2 : Theme.penWidth
        visible: !control.flat || control.down
    }

    popup: T.Popup {
        y: control.height
        width: control.width
        height: Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin)
        topMargin: 6
        bottomMargin: 6
        palette: control.palette

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0

            Rectangle {
                z: 10
                width: parent.width
                height: parent.height
                color: "transparent"
                border.color: Theme.borderColor
            }

            T.ScrollIndicator.vertical: C.ScrollIndicator { }
        }

        background: Rectangle {
            color: Theme.windowColor
        }
    }
}
