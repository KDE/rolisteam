import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Profile
import CustomItems
import Customization

Item {
    id: _root
    property alias model: _list.model
    property alias currentProfile: _list.currentIndex

    Action {
        id: _addProfile
        icon.name: "add-round"
        onTriggered: ProfileController.addProfile()
    }

    Action {
        id: _cloneProfile
        icon.name: "cloning"
        onTriggered: ProfileController.cloneProfile(_list.currentIndex)
    }


    Action {
        id: _removeProfile
        icon.name: "delete"
        onTriggered: ProfileController.removeProfile(_list.currentIndex)
    }

    SplitView {
        anchors.fill: parent
        Pane {
            SplitView.minimumWidth: 200 //list.contentWidth
            SplitView.preferredWidth: 200
            SplitView.maximumWidth: 400
            SplitView.fillHeight: true
            leftPadding: 2
            rightPadding: 2
            background: Rectangle {
                     color: palette.base
            }
            GridLayout {
                anchors.fill: parent
                columns: 5

                ListView {
                    id: _list
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.leftMargin: 0
                    Layout.columnSpan: 5
                    model: ProfileController.profileModel
                    currentIndex: ProfileController.currentProfileIndex
                    onCurrentIndexChanged: ProfileController.currentProfileIndex = _list.currentIndex //onsole.log("current changed: ",_list.currentIndex)

                    delegate: ItemDelegate {
                        text: model.name
                        width: _list.width
                        highlighted: ProfileController.currentProfileIndex === index
                        onClicked: _list.currentIndex = index
                    }

                    ScrollIndicator.vertical: ScrollIndicator { }

                }
                ToolButton {
                    id: addBtn
                    action: _addProfile
                    icon.width: 32
                    icon.height: 32
                    Layout.columnSpan: 2
                    icon.color: "transparent"
                    ToolTip.text: qsTr("New profile")
                    flat: true
                    opacity: addBtn.down ? 0.5 : 1.0
                }
                ToolButton {
                    id: cloneBtn
                    opacity: cloneBtn.down ? 0.5 : 1.0
                    action: _cloneProfile
                    icon.width: 32
                    icon.height: 32
                    ToolTip.text: qsTr("Clone profile")
                }
                ToolButton {
                    id: removeBtn
                    action: _removeProfile
                    Layout.columnSpan: 2
                    opacity: !enabled || removeBtn.down ? 0.5 : 1.0
                    icon.width: 32
                    icon.height: 32
                    icon.color: "transparent"
                    ToolTip.text: qsTr("Remove profile")
                    enabled: _list.count > 1 && ProfileController.currentProfileIndex >= 0
                }
            }
        }

        ScrollView {
            id: _scrollView
            SplitView.fillWidth: true

            ColumnLayout {
                id: _lyt
                width: _scrollView.width
                height: Math.max(_scrollView.height, _lyt.implicitHeight)
                spacing: 0
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: qsTr("Profile Name:")
                    }
                    RequiredTextField {
                        Layout.fillWidth: true
                        validInput: (text && ProfileController.profileName)
                        text: ProfileController.profileName
                        onTextChanged: ProfileController.profileName = text
                    }
                }
                GroupBox {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    title: qsTr("Connection")
                    GridLayout {
                        anchors.fill: parent
                        columns: 2
                        Label {
                            text: qsTr("Address:")
                        }
                        RequiredTextField {
                            Layout.fillWidth: true
                            text: ProfileController.address
                            validInput: (text && ProfileController.address || ProfileController.isServer)
                            enabled: !ProfileController.isServer
                            onEditingFinished: ProfileController.address = text
                        }
                        Label {
                            text: qsTr("Port:")
                        }
                        TextField {
                            Layout.fillWidth: true
                            validator: IntValidator {bottom: 1; top: 65535;}
                            text: ProfileController.port
                            onEditingFinished: ProfileController.port = text
                        }
                        Label {
                            text: qsTr("Password:")
                        }
                        TextField {
                            Layout.fillWidth: true
                            echoMode: TextField.Password
                            text: ProfileController.password
                            onEditingFinished: ProfileController.password = text
                        }
                        CheckBox {
                            id: _host
                            Layout.columnSpan: 2
                            text: qsTr("Host the game")
                            checked: ProfileController.isServer
                            onCheckedChanged: {
                                ProfileController.isServer =  _host.checked
                            }
                        }
                    }
                }
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("Player")
                    Layout.alignment: Qt.AlignTop
                    PersonEdit {
                        id: person
                        anchors.fill: parent
                        imageData: ProfileController.playerAvatar
                        characterName: ProfileController.playerName
                        validInput: (characterName && ProfileController.playerName)
                        color: ProfileController.playerColor
                        onColorEdited: (col)=> ProfileController.playerColor = col
                        onNameEdited: ProfileController.playerName = person.characterName
                        onClicked: ProfileController.selectPlayerAvatar()
                        CheckBox {
                            id: _gameMaster
                            Layout.columnSpan: 3
                            text: qsTr("I'm the Game Master")
                            checked: ProfileController.isGameMaster
                            onCheckedChanged: {
                                ProfileController.isGameMaster =  _gameMaster.checked
                            }
                        }
                        RowLayout {
                            Layout.columnSpan: 3
                            Layout.fillWidth: true
                            Label {
                                text: qsTr("Campaign Root:")
                                opacity: ProfileController.isGameMaster ? 1.0 : 0.4
                            }
                            RequiredTextField {
                                Layout.fillWidth: true
                                text: ProfileController.campaignPath
                                validInput: (text && ProfileController.campaignPath)
                                enabled: ProfileController.isGameMaster
                                opacity: enabled ? 1.0 : 0.4
                                onEditingFinished: {
                                    if(text !== ProfileController.campaignPath) {
                                        ProfileController.campaignPath = text
                                    }
                                    text = Qt.binding(function(){return ProfileController.campaignPath})
                                }

                            }
                            ToolButton {
                                icon.name: "folder"
                                enabled: ProfileController.isGameMaster
                                opacity: enabled ? 1.0 : 0.4
                                icon.color: "transparent"
                                onClicked: ProfileController.selectCampaignPath()
                            }
                        }
                    }
                }
                GroupBox {
                    id: _character
                    Layout.fillHeight: true
                    label: RowLayout {
                        Label {
                            text: qsTr("Characters")
                        }
                        ToolButton {
                            icon.name: "add"
                            icon.color: "transparent"
                            onClicked: ProfileController.addCharacter()
                        }
                    }
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredHeight: _characterList.height + _buttonBar.height + padding + padding
                    visible: !ProfileController.isGameMaster

                    ListView {
                        id: _characterList
                        width: _character.width
                        height: contentHeight
                        interactive: false

                        model: ProfileController.characterModel
                        delegate: ItemDelegate {
                            width: _characterList.width - _character.padding - _character.padding
                            height: _itemLyt.height

                            PersonEdit {
                                id: _itemLyt
                                imageData: model.avatarData
                                characterName: model.name
                                width: parent.width
                                color: model.color
                                property bool editAvatar: false
                                validInput: model.name && isSquare
                                onClicked:{
                                    console.log("Person:: On clickd")
                                    _itemLyt.editAvatar = true
                                    ProfileController.selectCharacterAvatar(model.index)
                                }
                                onNameEdited: ProfileController.editCharacterName(model.index,_itemLyt.characterName)
                                onColorEdited: (color)=>ProfileController.editCharacterColor(model.index,color)
                                onImageDataChanged: {
                                    console.log("Person:: ImageDatachanged")
                                    if(_itemLyt.editAvatar){
                                        ProfileController.editCharacterAvatar(model.index, _itemLyt.imageData)
                                        _itemLyt.editAvatar = false
                                    }
                                }
                                line: [
                                    ToolButton {
                                        icon.name: "remove"
                                        icon.color: "transparent"
                                        onClicked: ProfileController.removeCharacter(model.index)
                                        visible: index > 0
                                    }
                                ]
                            }
                        }

                    }
                }
                Label {
                    id: errorLbl
                    text: ProfileController.errorMsg
                    visible: ProfileController.errorMsg
                    background: Rectangle {
                        color: "red"
                    }

                }
                Label {
                    id: infoLbl
                    text: ProfileController.infoMsg
                    visible: ProfileController.infoMsg
                    background: Rectangle {
                        color: "green"
                    }
                }
                Item {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: ProfileController.isGameMaster
                }
                RowLayout {
                    id: _buttonBar
                    //Layout.alignment: Qt.AlignRight || Qt.AlignBottom
                    Layout.fillWidth: true
                    Button {
                        id: _saveProfiles
                        Layout.alignment: Qt.AlignLeft
                        text: qsTr("Save profiles")
                        onClicked: ProfileController.saveProfileModels()
                        enabled: true

                    }

                    Label {
                        Layout.fillWidth: true
                        text: ProfileController.canConnect ? "" : qsTr("Please, set data where you see red borders.")
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Button {
                        id: _connectBtn
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Connect")
                        focus: true
                        enabled: ProfileController.canConnect
                        onClicked: ProfileController.startConnection() //startNetworkConnection()
                        opacity: enabled ? 1.0 : 0.4
                        icon.name: _connectBtn.enabled ? "checked" : "close-circle"
                        icon.color: _connectBtn.enabled ? "green" : "red"
                    }
                    Button {
                        id: _cancelBtn
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Cancel")
                        onClicked: ProfileController.reject()
                    }
                }
            }
        }

    }

}
