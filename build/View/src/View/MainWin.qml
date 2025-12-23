// View/MainWin.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import "./RightContent"

ApplicationWindow {
    width: 1350
    height: 840
    visible: true
    title: "MVVM Example"

    property var pages: [
        "./RightContent/SearchPage.qml", 
        "./RightContent/SongList.qml", 
        "./RightContent/Ranking.qml", 
        "./RightContent/Favorites.qml", 
        "./RightContent/Settings.qml"]

    RowLayout{
        anchors.fill: parent

        SideBar {
            id: sidebar

            Layout.alignment: Qt.AlignTop
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            SearchBox {
                id: searchBox
            }

            // SearchPage{
            // }
            Loader{
                id: contentLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                source: pages[sidebar.selectedIndex]
            }
        }
    }
}
