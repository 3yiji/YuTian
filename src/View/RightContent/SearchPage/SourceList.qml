import QtQuick 6.10
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import ViewModel 1.0

Rectangle{
    id: header
    // color: '#d42222'
    color: "transparent"
    Layout.preferredHeight: sourceLabels.height *1.2
    Layout.fillWidth: true

    property SearchPageVM vm
    property int fontSize: 20
    property var sourceNames: [
        "小蜗音乐",
        "小枸音乐",
        "小秋音乐",
        "小芸音乐",
        "小蜜音乐",
    ]
    Component{
        id: inlineButton
        Rectangle{
            id: buttonRect
            color: "transparent"

            width: buttonLabel.width
            height: buttonLabel.height * 1.2
            
            property SearchPageVM vmRef
            property string propName
            property string textInput
            property int index
            property bool hovered: false
            property bool hadPlayed: false
            Label{
                id: buttonLabel
                text: textInput
                color: vmRef[propName] === index
                ? Theme.primary                // 选中
                : (hovered ? Theme.primary      // 悬停
                            : "black")
                font.pixelSize: header.fontSize

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
            }

            Rectangle{
                id: barImage
                radius: 4
                color: "transparent"

                width: buttonLabel.width
                height: buttonLabel.height * 0.2

                anchors.horizontalCenter: parent.horizontalCenter
            }

            MouseArea{
                id: buttonArea
                hoverEnabled: true
                onEntered: buttonRect.hovered = true
                onExited: buttonRect.hovered = false
                onClicked: vmRef[propName] = index

                anchors.fill: parent
            }

            // 绑定选中状态：父容器 selectedIndex 变化时触发动画
            onPropNameChanged: {
            // onCompleted:{
                if (vmRef) {
                    vmRef[propName + "Changed"].connect(onSelectedIndexChanged)
                }
                // vmRef[propName] = -1             // 手动触发初始化状态
                // vmRef[propName] = 0
            }

            ColorAnimation {
                id: colorAnim
                target: barImage
                property: "color"
                from: index === vmRef[propName] ? "transparent" : Theme.primary
                to: index === vmRef[propName] ? Theme.primary : "transparent"
                duration: 500
            }

            PropertyAnimation {
                id: moveAnim
                target: barImage  // 目标组件
                properties: "y"     // 要动画的属性（x 轴，也可以是 "x,y" 同时移动）
                from: index === vmRef[propName] ? buttonRect.height / 2 : buttonRect.height               // 起始位置（x=0）
                to: index === vmRef[propName] ? buttonRect.height : buttonRect.height / 2
                duration: 500      // 动画时长（1.5秒）
                easing.type: Easing.InOutQuad  // 缓动效果（先慢后快再慢，更自然）
                loops: 1            // 仅播放一次（可设为 Infinite 循环）
            }


            // 选中状态变化时，控制动画播放
            function onSelectedIndexChanged() {
                let isSelected = (vmRef && vmRef[propName] === index)
                if (isSelected) {
                    hadPlayed = true
                    colorAnim.stop()
                    colorAnim.start()

                    moveAnim.stop()
                    moveAnim.start()
                } 
                else if (hadPlayed) {
                    hadPlayed = false
                    colorAnim.stop()
                    colorAnim.start()

                    moveAnim.stop()
                    moveAnim.start()
                }
            }
        }
    }

    

    

    RowLayout{
        id: tabLabels
        spacing: 10
        
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        Repeater{
            model: ["歌单", "歌曲"]
            Loader{
                sourceComponent: inlineButton
                onLoaded: {
                    item.textInput = modelData
                    item.index = index
                    item.vmRef = vm
                    item.propName = "tabIndex"
                }
            }
        }
    }

    RowLayout{
        id: sourceLabels
        spacing: 10

        anchors.left: parent.left
        anchors.leftMargin: 10

        anchors.verticalCenter: parent.verticalCenter
        Repeater{
            model: sourceNames.length
            Loader{
                sourceComponent: inlineButton
                onLoaded: {
                    item.textInput = sourceNames[index]
                    item.index = index
                    item.vmRef = vm
                    item.propName = "srcIndex"
                }
            }
        }
    }
    
}

