import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: 200
    height: 1000
    property var repeaterModel: [
        "qrc:images/search.svg",
        "qrc:images/song list.svg",
        "qrc:images/ranking list.svg", 
        "qrc:images/collect.svg",
        "qrc:images/setting.svg", 
    ]

    Column {
        id: column
        x: 0
        y: 0
        width: 85
        height: 614

        Repeater {
            id: repeater
            model: repeaterModel

            Rectangle {
                id: rectangle
                width: 100
                height: 100
                color: "#ffffff"

                MouseArea {
                    id: mouseArea
                    width: 100
                    height: 100
                }
                Image {
                    id: image
                    width: 100
                    height: 100
                    source: modelData
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: width * 4
                    sourceSize.height: height * 4
                }
            }
        }

        BusyIndicator {
            id: busyIndicator
            hoverEnabled: false
            running: true
            baselineOffset: 6
            wheelEnabled: true
            focusPolicy: Qt.ClickFocus
            enabled: false
        }
    }
}
