import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Customization
import org.rolisteam.InstantMessaging

Drawer {
    id: control
    property int requiredWidth: pane.implicitWidth
    width: pane.implicitWidth
    height: pane.implicitHeight
    interactive: control.opened

    FontDialog{
        id: fontDial
        currentFont: InstantMessagerManager.ctrl.imFont
        onAccepted: {
            InstantMessagerManager.ctrl.imFont= fontDial.selectedFont
        }
    }
    onVisibleChanged: {
        if(visible)
            control.font = Theme.imFont
    }

    Pane {
        id: pane
            GridLayout {
                columns: 2
                   Label {
                        text: qsTr("Night Mode")
                        font: control.font
                        Layout.fillWidth: true
                    }
                    Switch {
                        id: nightSwitch
                        checked: InstantMessagerManager.ctrl.nightMode
                        onCheckedChanged: {
                            InstantMessagerManager.ctrl.nightMode = nightSwitch.checked
                        }
                    }

                    Label {
                        text: qsTr("Sound Notification")
                        font: control.font
                        Layout.fillWidth: true
                    }
                    Switch {
                        id: sound
                        checked: true
                        onCheckedChanged: {
                            InstantMessagerManager.ctrl.sound = sound.checked
                        }
                    }


                Button {
                    Layout.columnSpan: 2
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    text: qsTr("Select Font…")
                    font: control.font
                    onClicked: {
                        fontDial.open()
                    }
                }

                Label {
                    text: qsTr("Font Family:")
                    font: control.font
                }
                Label {
                    Layout.fillWidth: true
                    Layout.maximumWidth: 250
                    elide: Text.ElideRight
                    text: InstantMessagerManager.ctrl.imFont.family
                    font: control.font
                }
                Label {
                    text: qsTr("Font size Factor:")
                    font: control.font
                }
                Label {
                    text: Theme.fontSizeFactor.toFixed(2)
                }
                    Slider {
                        id: sizeFactor
                        from: 0.2
                        value: Theme.fontSizeFactor
                        Layout.fillWidth: true
                        to: 5.0
                        onMoved: {
                            if(Theme.fontSizeFactor !== sizeFactor.value)
                                Theme.fontSizeFactor = sizeFactor.value
                        }
                    }
                    ToolButton {
                        icon.name: "reset"
                        onClicked: Theme.fontSizeFactor = 1.0
                    }
            }
    }
}
