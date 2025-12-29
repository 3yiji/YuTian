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

    ColumnLayout{
        anchors.fill: parent

        SourceList{
            vm: vm
        }

        Loader{
            id: resultsLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: vm.isSearching ? "./Loading.qml" : pages[vm.tabIndex]

            property var pages: [
                "./SongList/SongList.qml",               // 歌曲页面
                "./SongListList.qml",           // 歌单页面
            ]
            onLoaded: {
                if (item && item.hasOwnProperty("vm")) {
                    item.vm = vm; // 给子页面赋值 vm
                }
            }
        }
        
    }

}