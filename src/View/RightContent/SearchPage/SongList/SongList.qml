import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import ViewModel 1.0

Rectangle { 

    property SearchPageVM vm
    height: 100
    width: 100

    ScrollView {
        id: songScrollView
        anchors.fill: parent
        clip: true  // 裁剪超出部分

        
        ColumnLayout {
            width: songScrollView.availableWidth
            // width: songScrollView.width
            spacing: 0

            InfoBox {
                id: infoBoxHeader
                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                
                index: "#"
                name: "歌曲名"
                singer: "艺术家"
                album: "专辑名"
                interval: "时长"
                operation: "操作"
            }
            // 示例内容
            Repeater {
                model: vm.songList && vm.songList.length > 0 ? vm.songList : []
                Rectangle {
                    Layout.fillWidth: true
                    // Layout.preferredHeight: infoBox.Height
                    Layout.preferredHeight: infoBox.implicitHeight * 1.5
                    color: index % 2 === 0 ? "#F5F5F5" : "#FFFFFF"
                    InfoBox {
                        id: infoBox
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        // height: parent.height
                        // width: parent.width
                        index: modelData.index
                        name: modelData.name
                        singer: modelData.singer
                        album: modelData.album
                        interval: modelData.interval
                    }
                }

                
            }
            Label{
                text: "总歌曲数：" + vm.songList.length
            }
            // 分页组件
            Pagination {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                totalPages: 20
                onPageChanged: {
                    console.log("切换到第", page, "页")
                    // vm.loadPage(page)
                    console.log(`vm.songList[0].name:, ${vm.songList[0].name}`);
                }
            }
        }
    }
    
}