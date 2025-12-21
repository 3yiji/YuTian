import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    width: 100
    height: 1000
    color: "#F3FBF8"              // 侧边栏整体浅绿背景
    property int selectedIndex: -1
    property var repeaterModel: [
        "qrc:/images/search.svg",
        "qrc:/images/song list.svg",
        "qrc:/images/ranking list.svg",
        "qrc:/images/collect.svg",
        "qrc:/images/setting.svg"
    ]

    Column {
        id: column
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // 顶部 Logo
        Text {
            text: "L X"
            font.pixelSize: 60
            font.bold: true
            color: "#6BBF8E"
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
            anchors.topMargin: 8
        }

        Repeater {
            id: repeater
            model: repeaterModel

            Rectangle {
                id: itemBg
                width: column.width - 0
                height: 90
                radius: 6
                // 悬停与选中高亮
                property bool hovered: false
                color: selectedIndex === index
                       ? "#CBE8DA"                 // 选中
                       : (hovered ? '#e0bf1a'      // 悬停
                                  : "transparent")

                // Behavior on color { ColorAnimation { duration: 120 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: itemBg.hovered = true
                    onExited: itemBg.hovered = false
                    onClicked: selectedIndex = index
                }

                Image {
                    anchors.centerIn: parent
                    width: 60
                    height: 60
                    source: modelData
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: width * 4
                    sourceSize.height: height * 4
                }
            }
        }
    }
}