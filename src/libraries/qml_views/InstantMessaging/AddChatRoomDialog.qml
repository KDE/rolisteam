import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    property alias model: contactList.model
    property alias all: everyone.checked
    property alias title: title.text
    property list<string> recipiants: []
    property bool emptyRecipiants: true

    signal chatRoomAdded()
    width: frame.implicitWidth
    height: frame.implicitHeight
    padding: 0

    Frame {
        id: frame
        ColumnLayout {
            id: layout
            anchors.fill: parent
            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: qsTr("Title")
                }
                TextField {
                    id: title
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Label{
                    text: qsTr("Everybody")
                }
                Switch {
                    id: everyone
                    objectName: "everyone"
                    checked: true
                }
            }
            Frame {
                id: userlist
                Layout.fillWidth: true
                enabled: !everyone.checked
                ColumnLayout {
                    anchors.fill: parent
                    Repeater {
                        id: contactList
                        RowLayout {
                            Layout.fillWidth: true
                            visible: !model.local
                            Image {
                                source: "image://avatar/%1".arg(model.uuid)
                                fillMode: Image.PreserveAspectFit
                                sourceSize.width: 50
                                sourceSize.height: 50
                                opacity: everyone.checked ? 0.5 : 1.0
                            }
                            Label {
                                text: model.name
                                Layout.fillWidth: true
                            }
                            Switch {
                                enabled: !everyone.checked
                                objectName: model.name
                                onCheckedChanged: {
                                    if(checked)
                                    {
                                        root.recipiants.push(model.uuid)
                                    }
                                    else
                                    {
                                        root.recipiants.splice(root.recipiants.indexOf(model.uuid),1)
                                    }
                                   root.emptyRecipiants = (root.recipiants.length == 0)
                                }
                            }
                        }
                    }
                }
            }

            Button {
                //: translator please, Keep it short.
                text: qsTr("Add Chatroom")
                Layout.alignment: Qt.AlignRight
                enabled: title.text.length > 0 && (everyone.checked || !root.emptyRecipiants )
                onClicked: {
                    root.recipiants.push(model.localPlayerId)
                    root.chatRoomAdded()
                    root.close()
                }
            }
        }
    }
}
