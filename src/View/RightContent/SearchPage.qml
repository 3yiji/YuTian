import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle { 
    color: "#FFFFFF"; 
    


    Layout.fillWidth: true
    Layout.fillHeight: true

    property var sourceNames: [
        "小蜗音乐",
        "小枸音乐",
        "小秋音乐",
        "小芸音乐",
        "小蜜音乐",
        "聚合大会",
    ]

    property int selectedSourceIndex: 0
    property int selectedTabIndex: 0

    ColumnLayout{
        anchors.fill: parent
        Rectangle{
            id: header
            color: '#d42222'
            Layout.preferredHeight: fontSize + 8
            Layout.fillWidth: true

            property int fontSize: 20
            RowLayout{
                id: "sourceLabels"
                spacing: 10

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                Repeater{
                    model: sourceNames.length
                    Label{
                        text: sourceNames[index]
                        color: Theme.primary
                        font.pixelSize: header.fontSize
                    }
                }
            }

            RowLayout{
                id: "tabLabels"

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                Repeater{
                    model: ["歌单", "歌曲"]
                    Label{
                        text: modelData
                        font.pixelSize: header.fontSize
                    }
                }
            }
            
        }

        Loader{
            id: resultsLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: pages[0]

            property var pages: [
                "./searchpage/Song.qml",
                "./searchpage/SongList.qml",
            ]
        }
    }
}