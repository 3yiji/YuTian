import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

import ViewModel 1.0

Rectangle { 
    color: "#FFFFFF"; 

    Layout.fillWidth: true
    Layout.fillHeight: true

    SearchPageVM{
        id: vm

    }

    Connections{
        target: EventBus
        function onSearchRequested(keyword) {   // 函数式写法，参数名匹配信号
            vm.performSearch(keyword)
        }
    }

    ColumnLayout{
        anchors.fill: parent

        SourceList{
        }

        Loader{
            id: resultsLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: pages[0]

            property var pages: [
                "./SongList.qml",               // 歌曲页面
                "./SongListList.qml",           // 歌单页面
            ]
        }
        
    }
}