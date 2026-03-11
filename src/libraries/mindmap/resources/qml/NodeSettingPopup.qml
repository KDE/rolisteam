import QtQuick
import QtCore
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import mindmap
import CustomItems
import mindmapcpp


Popup {
    id: _root
    modal: true
    property MindNode node
    required property MindMapController ctrl
    property bool hasAvatar : false

    implicitHeight: flickable.contentHeight + 2*_root.padding

    FileDialog {
        id: openDialog
        title: qsTr("Choose Image...")
        nameFilters: ["Images (*.png *.jpg *.jpeg *.svg *.gif)"]

        onAccepted: {
            var data="";
            _root.ctrl.addImageFor(_root.node.id, openDialog.selectedFile, data)
        }
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
    }

    Dialog {
        id: urlDialog
        title: qsTr("Set image url")
        standardButtons: img.status == Image.Ready ?  Dialog.Ok | Dialog.Cancel : Dialog.Cancel

        ColumnLayout {
            RowLayout {
                Label {
                    text: qsTr("Url")
                }
                TextField {
                    id: urlEdit
                }
            }
            Image {
                id: img
                Layout.fillHeight: true
                Layout.fillWidth: true
                source: urlEdit.text
            }
        }
        onAccepted: {
            var data="";
            _root.ctrl.addImageFor(_root.node.id, urlEdit.text, data)
        }
        //onRejected: console.log("Cancel clicked")
    }

    ScrollView {
        id: flickable
        anchors.fill: parent
        contentWidth: mainLyt.implicitWidth
        contentHeight: mainLyt.implicitHeight
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        clip: true
        ColumnLayout {
            id: mainLyt
            spacing: 2

            GridLayout {
                id: info
                columns: 1
                Layout.fillWidth: true
                Layout.rightMargin: 15
                Label {
                    text: qsTr("Description: ")
                }
                ScrollView {
                    id: view
                    Layout.rowSpan: 3
                    Layout.fillWidth: true
                    TextArea {
                        implicitHeight: 150
                        placeholderText: qsTr("Description")
                        text: _root.node ? _root.node.description : ""
                        onEditingFinished: _root.node.description = text
                    }
                }
                Label {
                    text: qsTr("tags:")
                }
                TextField {
                    text:_root.node ? _root.node.tagsText : ""
                    Layout.fillWidth: true
                    onEditingFinished: _root.node.tagsText = text
                }
                Label {
                    text: qsTr("Avatar:")
                }
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: qsTr("Load")
                        onClicked: openDialog.open()
                    }
                    Button {
                        text: qsTr("Set URL")
                        visible: false
                        onClicked: urlDialog.open()
                    }
                    Button {
                        text: qsTr("Remove")
                        enabled:  _root.hasAvatar
                        onClicked: _root.ctrl.removeImageFor(_root.node.id)
                    }
                }
            }

            GroupBox {
                id: styleTab
                title: qsTr("Node Styles")
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.rightMargin: 30
                GridLayout {
                    id: grid
                    columns: 3
                    anchors.fill: parent
                    Repeater {
                        model: _root.ctrl.styleModel
                        Button {
                            id: control
                            Layout.preferredWidth: 120
                            Layout.preferredHeight: 30

                            background: Rectangle {
                                radius: 8
                                border.width: 1
                                border.color: "black"
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: control.pressed ? colorTwo : colorOne }//
                                    GradientStop { position: 1.0; color: control.pressed ? colorOne : colorTwo }//
                                }

                            }
                            contentItem: Text {
                                color: textColor
                                text: qsTr("Text")
                                horizontalAlignment: Text.AlignHCenter
                            }
                            onClicked: {
                                _root.node.styleIndex = index
                                _root.close()
                            }
                        }
                    }
                }
            }
        }
    }

}
