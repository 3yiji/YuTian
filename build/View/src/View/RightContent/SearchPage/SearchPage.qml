import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle { 
    color: "#FFFFFF"; 

    Layout.fillWidth: true
    Layout.fillHeight: true

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
                "./Song.qml",
                "./SongList.qml",
            ]
        }
        
    }
}