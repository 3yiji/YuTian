// View/MainWin.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    width: 1350
    height: 840
    visible: true
    title: "MVVM Example"

    Row{
        anchors.fill: parent
        SideBar {
            id: sidebar
            width: 157
            height: 840
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }

        Column{
            anchors.left: sidebar.right
            anchors.top: parent.top
            SearchBox {
                id: searchBox
                width: 480
                height: 40
            }
            Loader{
                id: contentLoader
                width: 1193
                height: 760
                sourceComponent: Rectangle{
                    color: "#FFFFFF"}
            }
        }
    }
}
