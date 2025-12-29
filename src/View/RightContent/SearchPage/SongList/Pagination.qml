import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: pagination
    color: "transparent"
    height: 40

    property int currentPage: 1      // 当前页
    property int totalPages: 20      // 总页数
    property int visiblePages: 7     // 显示多少个页码

    signal pageChanged(int page)

    RowLayout {
        // anchors.centerIn: parent
        
        spacing: 5

        // 上一页按钮
        Rectangle {
            width: 30; height: 30
            color: "transparent"
            border.color: "#E0E0E0"
            radius: 4

            Label {
                text: "<"
                anchors.centerIn: parent
                color: currentPage > 1 ? "#666666" : "#CCCCCC"
            }

            MouseArea {
                anchors.fill: parent
                enabled: currentPage > 1
                onClicked: {
                    currentPage--
                    pageChanged(currentPage)
                }
            }
        }

        // 页码按钮
        Repeater {
            model: {
                var pages = []
                var start = Math.max(1, currentPage - Math.floor(visiblePages / 2))
                var end = Math.min(totalPages, start + visiblePages - 1)
                start = Math.max(1, end - visiblePages + 1)
                
                for (var i = start; i <= end; i++) {
                    pages.push(i)
                }
                return pages
            }

            delegate: Rectangle {
                width: 30; height: 30
                color: modelData === currentPage ? "#4CAF50" : "transparent"
                border.color: modelData === currentPage ? "#4CAF50" : "#E0E0E0"
                radius: 4

                Label {
                    text: modelData
                    anchors.centerIn: parent
                    color: modelData === currentPage ? "white" : "#666666"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        currentPage = modelData
                        pageChanged(currentPage)
                    }
                }
            }
        }

        // 跳转到最后几页
        // Rectangle {
        //     width: 30; height: 30
        //     color: "transparent"
        //     visible: currentPage < totalPages - visiblePages / 2

        //     Label {
        //         text: ">>"
        //         anchors.centerIn: parent
        //         color: "#4CAF50"
        //     }

        //     MouseArea {
        //         anchors.fill: parent
        //         onClicked: {
        //             currentPage = totalPages
        //             pageChanged(currentPage)
        //         }
        //     }
        // }

        // 下一页按钮
        Rectangle {
            width: 30; height: 30
            color: "transparent"
            border.color: "#E0E0E0"
            radius: 4

            Label {
                text: ">"
                anchors.centerIn: parent
                color: currentPage < totalPages ? "#666666" : "#CCCCCC"
            }

            MouseArea {
                anchors.fill: parent
                enabled: currentPage < totalPages
                onClicked: {
                    currentPage++
                    pageChanged(currentPage)
                }
            }
        }
    }
}