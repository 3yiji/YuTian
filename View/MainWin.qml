// View/MainWin.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    width: 1350
    height: 840
    visible: true
    title: "MVVM Example"

    BusyIndicator {
        id: busyIndicator
        x: 1079
        y: 327
    }

    SideBar {
        id: sidebar
        x: 0
        y: 0
        width: 157
        height: 840
    }
//     Column {
//         anchors.centerIn: parent
//         spacing: 20

//         Text {
//             text: "Counter: " + viewModel.counter5
//             font.pixelSize: 22
//         }

//         Button {
//             text: "Add"
//             onClicked: viewModel.increment()
//         }

//         Timer {
//             interval: 1000
//             running: true
//             repeat: true
//             onTriggered: console.log("counter:", viewModel.counter)
// }
//     }
}
