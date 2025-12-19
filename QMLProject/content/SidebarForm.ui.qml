

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: 200
    height: 1000
    property alias repeaterModel: repeater.model

    Column {
        id: column
        x: 0
        y: 0
        width: 85
        height: 614

        Repeater {
            id: repeater

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
