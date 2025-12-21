import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 480
    height: 40
    radius: 8
    color: hovered || tf.activeFocus ? "#E6F5EE" : "#DDF1E8"
    border.color: "#CBE8DA"
    border.width: 0
    property alias text: tf.text
    signal searchRequested(string query)
    property bool hovered: false
    Behavior on color { ColorAnimation { duration: 120 } }

    // 鼠标悬停效果
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onClicked: tf.focus = true
    }

    // 文本输入
    TextInput {
        id: tf
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 40     // 留出右侧图标空间
        verticalAlignment: Text.AlignVCenter
        clip: true
        font.pixelSize: 16
        color: "#5FAF86"
        selectByMouse: true
        onAccepted: root.searchRequested(text)
        // 无背景，让外层 Rectangle 负责绘制
    }

    // 自定义占位文本（绿色）
    Text {
        text: "Search for something..."
        anchors.verticalCenter: tf.verticalCenter
        anchors.left: tf.left
        color: "#5FAF86"
        opacity: tf.text.length === 0 ? 0.8 : 0
        Behavior on opacity { NumberAnimation { duration: 120 } }
        // 当输入非空时隐藏
    }

    // 清除按钮（×）
    Text {
        id: clearBtn
        text: "✕"
        font.pixelSize: 18
        color: "#5FAF86"
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: icon.left
        anchors.rightMargin: 12
        opacity: tf.text.length > 0 ? 0.85 : 0
        visible: tf.text.length > 0
        Behavior on opacity { NumberAnimation { duration: 100 } }

        MouseArea {
            anchors.fill: parent
            onClicked: { tf.text = ""; tf.forceActiveFocus() }
            hoverEnabled: true
            onEntered: clearBtn.opacity = 1.0
            onExited: clearBtn.opacity = 0.85
        }
    }

    // 右侧搜索图标
    Image {
        id: icon
        source: "qrc:/images/search.svg"
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        width: 20; height: 20
        fillMode: Image.PreserveAspectFit
        opacity: 0.85
    }
    MouseArea {
        anchors.fill: icon
        onClicked: root.searchRequested(tf.text)
        hoverEnabled: true
        onEntered: icon.opacity = 1.0
        onExited: icon.opacity = 0.85
    }
}