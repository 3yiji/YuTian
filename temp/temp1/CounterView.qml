import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    width: 200
    height: 200
    
    property alias viewModel: root.viewModel
    
    Column {
        anchors.centerIn: parent
        spacing: 20
        
        Label {
            id: valueLabel
            text: viewModel ? viewModel.value : "0"
            font.pixelSize: 24
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        Button {
            text: "+"
            onClicked: viewModel.increment()
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        Button {
            text: "Reset"
            onClicked: viewModel.reset()
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
    
    Connections {
        target: viewModel
        function onValueChanged() {
            valueLabel.text = viewModel.value
        }
    }
} 