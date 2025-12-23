import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    color: "#F3FBF8"              // 侧边栏整体浅绿背景
    // color: '#972ad1'

    // Layout.fillHeight: true
    Layout.preferredWidth: 80
    Layout.preferredHeight: column.height

    // Component.onCompleted: {
    //     console.log("Layout.preferredHeight", Layout.preferredHeight)       // 打印当前 Rectangle 的实际宽度
    //     console.log("Layout.preferredWidth", Layout.preferredWidth)     // 打印实际高度（等于宽度，正方形）
    // }

    // Timer {
    //     id: printTimer
    //     interval: 2000  // 每 2 秒打印一次（可调整）
    //     running: true   // 组件加载后自动启动
    //     repeat: true    // 循环打印（false 仅打印一次）

    //     // 定时器触发时执行打印
    //     onTriggered: {
    //         console.log("root.Layout.preferredHeight", root.Layout.preferredHeight)       // 打印当前 Rectangle 的实际宽度
    //         console.log("root.Layout.preferredWidth", root.Layout.preferredWidth)     // 打印实际高度（等于宽度，正方形）
    //         console.log("root.height", root.height)       // 打印当前 Rectangle 的实际宽度
    //         console.log("root.width", root.width)     // 打印实际高度（等于宽度，正方形）
    //         console.log("column.height", column.height)       // 打印当前 Rectangle 的实际宽度
    //         console.log("column.width", column.width)     // 打印实际高度（等于宽度，正方形）
    //     }
    // }

    property int selectedIndex: 0
    property var repeaterModel: [
        "qrc:/images/search.svg",
        "qrc:/images/song list.svg",
        "qrc:/images/ranking list.svg",
        "qrc:/images/collect.svg",
        "qrc:/images/setting.svg"
    ]

    ColumnLayout {
        id: column
        width: parent.width
        // anchors.fill: parent

        spacing: 0


        Text {
            text: "L X"
            font.pixelSize: 40
            font.bold: true
            color: "#6BBF8E"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            // color: '#27be8c'

            Layout.fillWidth: true
            Layout.preferredHeight: width
        }

        Repeater {
            id: repeater
            model: repeaterModel

            Rectangle {
                id: itemBg
                radius: 6
                clip: true

                Layout.fillWidth: true
                Layout.preferredHeight: width

                // 悬停与选中高亮
                property bool hovered: false
                color: selectedIndex === index
                       ? "#CBE8DA"                 // 选中
                       : (hovered ? '#CBE8DA'      // 悬停
                                  : "transparent")

                // Behavior on color { ColorAnimation { duration: 120 } }

                

                Image {
                    source: modelData
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: width * 4
                    sourceSize.height: height * 4

                    width: parent.width * 0.5
                    height: parent.height * 0.5
                    anchors.centerIn: parent
                }

                MouseArea {
                    hoverEnabled: true
                    onEntered: itemBg.hovered = true
                    onExited: itemBg.hovered = false
                    onClicked: selectedIndex = index

                    height: parent.height
                    width: parent.width
                }
            }
        }
    }
}