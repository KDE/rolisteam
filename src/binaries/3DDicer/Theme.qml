pragma Singleton
import QtQuick.Controls.Material
import QtQml
import QtQuick
import dicely

QtObject {
    id: root

    Material.theme: DiceMainController.darkMode ? Material.Dark : Material.Light
    readonly property bool d : DiceMainController.darkMode

    readonly property int iconSize: 30
    readonly property int tabIconSize: 60
    readonly property int titleFontSize: 50
    readonly property int commandFontSize: 20
    readonly property int finalDiceResultFontSize: 30
    readonly property int textFieldFontSize: 30
    readonly property int timeFontSize:8
    readonly property int margin: 8
    readonly property int padding: 8
    readonly property int spacing: 8
    readonly property int radius: 10
    readonly property int penWidth: 1
    readonly property int colorButtonSize: 50
    readonly property int minimalTextField: 30

    // color
    readonly property color textColor: Material.Brown //"black"
    readonly property color borderColor: Material.Brown //"black"
    readonly property color disabledColor : Material.Red //"#66DC143C"
    readonly property color selectedColor : Material.LightGreen //"#662e8b57"
    readonly property color transparent : "transparent"
    readonly property color deleteTextColor: root.d ? "black" : "white"
    readonly property color deleteBtnColor: Material.DeepOrange //"tomato"
}
