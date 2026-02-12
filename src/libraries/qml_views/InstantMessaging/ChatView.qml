import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import InstantMessaging
import Customization
import org.rolisteam.InstantMessaging

Item {
    id: root
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")
    property QtObject paletteSheet: Theme.styleSheet("Palette")
    property alias localPersonModel: imEditText.model
    property alias chatroomModel: repeater.model
    property bool isMergeable: true
    property int chatRoomCount: 0
    property int chatRoomIndex: 0
    property ChatRoom chatRoom //tabHeader.currentItem.chatroom//:  tabHeader.currentIndex >= 0 ? chatroomModel.get(tabHeader.currentIndex) : null
    property alias tabBarHeight: tabHeader.height
    property int tabBarRightMargin: 0
    signal zoomChanged(var delta)
    signal addChat(string title, bool all, list<string> recipiants)
    signal split(string uuid, int index)
    signal merge(string uuid)
    signal moveRight(string uuid)
    signal moveLeft(string uuid)

    SoundEffect {
        id: effect
        source: "qrc:/resources/sounds/Doorbell.wav"
        muted: !InstantMessagerManager.ctrl.sound
        volume: 1.0
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: (mouse)=>{
            if (mouse.button === Qt.RightButton)
                contextMenu.popup()
        }
        onPressAndHold:(mouse)=> {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        onWheel:  (wheel) => {//(wheel)
            if (wheel.modifiers & Qt.ControlModifier) {
                zoomChanged(wheel.angleDelta.y / 240)
            }
        }

        Menu {
            id: contextMenu
            property string currentChat: root.chatRoom ? root.chatRoom.title : ""
            Action {
                text: qsTr("Split %1".arg(contextMenu.currentChat))
                enabled: tabHeader.count > 1
                onTriggered: {
                    root.split(tabHeader.currentItem.uuid, tabHeader.currentIndex)
                    root.chatRoom =  chatroomModel.get(tabHeader.currentIndex)
                }
            }
            Action  {
                text: qsTr("Merge %1".arg(contextMenu.currentChat))
                enabled: root.isMergeable
                onTriggered: {
                    root.merge(tabHeader.currentItem.uuid)
                }
            }
            Action  {
                text: qsTr("Move %1 Left".arg(contextMenu.currentChat))
                enabled: root.chatRoomIndex > 0 && (tabHeader.count > 1)
                onTriggered: {
                    root.moveLeft(tabHeader.currentItem.uuid)
                }
            }
            Action  {
                text: qsTr("Move %1 Right".arg(contextMenu.currentChat))
                enabled: (root.chatRoomIndex + 1 < root.chatRoomCount) && (tabHeader.count > 1)
                onTriggered: {
                    root.moveRight(tabHeader.currentItem.uuid)
                }
            }
        }
    }

    ColumnLayout {
        id: mainLyt
        anchors.fill: parent
        RowLayout {
            Layout.fillWidth: true
            TabBar {
                id: tabHeader

                Layout.fillWidth: true
                Layout.rightMargin: root.tabBarRightMargin

                Repeater {
                    id: repeater
                    TabButton {
                        id: tabButton
                        required property string title
                        required property string uuid
                        required property int index
                        required property bool closable
                        required property bool unread
                        required property ChatRoom chatroom

                        property bool current: tabHeader.currentIndex === tabButton.index
                        onCurrentChanged: {
                            if(current) {
                                root.chatRoom = tabButton.chatroom
                            }
                        }

                        text: tabButton.title
                        objectName: tabButton.title
                        background: Rectangle {
                            color: tabButton.current ? root.paletteSheet.alternateBase : tabButton.unread ? root.paletteSheet.highlight : root.paletteSheet.mid
                        }

                        onClicked: {
                            tabHeader.currentIndex = tabButton.index
                            root.chatRoom = tabButton.chatroom
                        }

                        contentItem: RowLayout {
                            Label {
                                text: (tabButton.unread ? "%1 (\*)".arg(tabButton.title) : tabButton.title)
                                Layout.fillWidth: true
                                horizontalAlignment: Qt.AlignHCenter
                                font: Theme.imFont
                                color: tabButton.current ? root.paletteSheet.text : root.paletteSheet.button
                            }
                            ToolButton {
                                visible: tabButton.closable
                                text: "X"
                                ToolTip.text: qsTr("close")
                                ToolTip.visible: down
                                onClicked: {
                                    InstantMessagerManager.ctrl.closeChatroom(tabButton.uuid, false)
                                }
                            }
                        }
                        Connections {
                            target: tabButton.chatroom
                            function onUnreadMessageChanged(unread) {
                                if(unread && !tabButton.current)
                                    effect.play()
                            }
                        }
                    }
                }
            }
        }

        SplitView {
            id: mainView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: root.styleSheet.sideMargin
            Layout.rightMargin: root.styleSheet.sideMargin
            orientation: Qt.Vertical

            SplitView {

                orientation: Qt.Horizontal
                SplitView.fillWidth: true
                SplitView.fillHeight: true

                ListView {
                    id: listView
                    SplitView.fillWidth: true
                    SplitView.fillHeight: true
                    objectName: "chatroom_%1".arg(root.chatRoomIndex)
                    model: root.chatRoom ? root.chatRoom.messageModel: 0
                    clip: true
                    spacing: 5
                    verticalLayoutDirection: ListView.BottomToTop
                    delegate: Component {
                        id: delegateComponent
                        Loader {
                            property string writerIdldr: model.writerId
                            property string messageTextldr: model.text
                            property bool localldr: model.local
                            property string timeldr: model.time
                            property string writerNameldr: model.writerName
                            property real windowWidthldr: listView.width
                            property url imageLinkldr: model.imageLink ?? ""

                            property bool isTextMessage: model.type === MessageInterface.Text
                            property bool isDiceMessage: model.type === MessageInterface.Dice
                            property bool isCommandMessage: model.type === MessageInterface.Command
                            property bool isErrorMessage: model.type === MessageInterface.Error
                            property bool mustBeOnTheRight: model.local && (isTextMessage || isCommandMessage)
                            anchors.right: mustBeOnTheRight && parent ? parent.right : undefined
                            width: listView.width-10 //(isDiceMessage || isErrorMessage) ?  parent.width-10 : undefined
                            source: isTextMessage ? "TextMessageDelegate.qml" :
                                    isCommandMessage ? "CommandMessageDelegate.qml" :
                                    isDiceMessage ? "DiceMessageDelegate.qml" : "ErrorMessageDelegate.qml"
                        }
                    }
                }
                RecipiantsView {
                    id: recipiantsView
                    visible: false
                    chatRoom: root.chatRoom
                    SplitView.fillHeight: true
                    SplitView.minimumWidth: visible ? 100 : 0
                }
            }

            InstantMessagingEditText {
                id: imEditText
                SplitView.fillWidth: true
                SplitView.preferredHeight: root.styleSheet.preferredHeight

                onSendClicked: (text, imageLink) => {
                   root.chatRoom.addMessage(text,imageLink, imEditText.currentPersonId, imEditText.currentPersonName)
                }
                function updateUnread() {
                    root.chatRoom.unreadMessage = false
                }
                onFocusGained: updateUnread()
            }

        }
    }

    ToolButton {
        id: roomSettings
        anchors.top: mainLyt.top
        anchors.right: mainLyt.right
        anchors.topMargin: 12 + tabHeader.height
        anchors.rightMargin: 12
        checked: recipiantsView.visible
        checkable: true
        icon.source: "qrc:/resources/rolistheme/05_rooms2.svg"
        icon.color: checked ?  "transparent" : "#80808080"
        icon.height: 24
        icon.width: 24
        onClicked: {
            recipiantsView.visible = !recipiantsView.visible

            recipiantsView.width = recipiantsView.visible ? 100 : 0
        }
    }
}
