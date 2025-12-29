import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

// 单首歌曲项：按权重布局
RowLayout {

    spacing: 10  // 子项间距


    property int index: 18
    property string name: "彩虹(Live) SQ"
    property string singer: "伍佰"
    property string album: "爱上巧克力 电视原声带"
    property string duration: "03:55"

    property int fontPixelSize: 18

    // 1. 序号（固定宽）
    Label {
        text: index
        font.pixelSize: fontPixelSize
        Layout.preferredWidth: 100  // 固定宽度
        Layout.fillWidth: true
        Layout.minimumWidth: 30  // 最小宽度
        verticalAlignment: Text.AlignVCenter
    }

    // 2. 歌曲名（占主要宽度，权重最高）
    Label {
        text: name
        font.pixelSize: fontPixelSize
        Layout.preferredWidth: 300  // 固定宽度
        Layout.fillWidth: true
        Layout.minimumWidth: 100  // 最小宽度
        elide: Text.ElideRight  // 内容过长时省略
        verticalAlignment: Text.AlignVCenter
    }

    // 3. 歌手（权重次之）
    Label {
        text: singer
        font.pixelSize: fontPixelSize
        Layout.preferredWidth: 300  // 固定宽度
        Layout.fillWidth: true
        Layout.minimumWidth: 100  // 最小宽度
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }

    // 4. 专辑（权重较低）
    Label {
        text: album
        font.pixelSize: fontPixelSize
        Layout.preferredWidth: 300  // 固定宽度
        Layout.fillWidth: true
        Layout.minimumWidth: 100  // 最小宽度
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }

    // 5. 时长（固定宽）
    Label {
        text: duration
        font.pixelSize: fontPixelSize
        Layout.preferredWidth: 100  // 固定宽度
        Layout.fillWidth: true
        Layout.minimumWidth: 100  // 最小宽度
        verticalAlignment: Text.AlignVCenter
    }

    RowLayout{
        // Layout.fillWidth: 0.1
    
        // 6. 耳机图标（固定宽）
        Image {
            source: "qrc:/images/headphones.svg"
            Layout.preferredWidth: fontPixelSize*1.2
            Layout.preferredHeight: fontPixelSize*1.2
            // color: "#4CAF50"
        }

        // 7. +号图标（固定宽）
        Image {
            source: "qrc:/images/plus.svg"
            Layout.preferredWidth: fontPixelSize*1.2
            Layout.preferredHeight: fontPixelSize*1.2
            // color: "#4CAF50"
        }
    }
}