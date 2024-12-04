import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.example.client 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 650
    height: 565
    title: "Управление Мультиметром"
    
    property bool isConnected: false // Флаг состояния подключения к серверу
    property bool autoUpdate: true  // Флаг для автоматического обновления

    Client {
        id: client        
    }

    Timer {
        id: updateTimer
        interval: 2500 
        running: autoUpdate
        repeat: true
        onTriggered: {
            isConnected = true;
            for (let i = 0; i < channelRepeater.count; i++) {
                let item = channelRepeater.itemAt(i);
                let result = client.sendCommand("get_result channel" + i);
                item.updateMeasurementValue(result);
            }            
        }
    }

    header: ToolBar {
        contentHeight: 40
        RowLayout {
            anchors.fill: parent
            spacing: 10

            Text {
                text: "   Статус сервера: "
                color: "black"
                font.bold: true
            }
            Rectangle {
                id: serverStatus
                width: 20
                height: 20
                color: isConnected ? "green" : "red" 
                radius: 10
            }
            Button {
                text: isConnected ? "Disconnect" : "Connect"
                onClicked: {
                    if (!isConnected) {
                        console.log("Подключение к серверу...");
                        client.connectToServer(); 
                        isConnected = true;
                    } else {                        
                        console.log("Отключение от сервера...");
                        isConnected = false;
                    }
                }
            }
        }
    }

    GridLayout {
        id: channelLayout
        columns: 2
        anchors.fill: parent
        anchors.margins: 20

        Repeater {
            id: channelRepeater
            model: 4 

            delegate: Item {
                id: itemDelegate
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 300
                Layout.preferredHeight: 250

                Rectangle {
                    width: 300
                    height: 250
                    color: "#f0f0f0"
                    radius: 10
                    border.color: "#cccccc"
                    border.width: 1

                    Column {
                        anchors.centerIn: parent
                        spacing: 10

                        Text {
                            id: channelTitle
                            text: "Канал " + modelData
                            font.bold: true
                            font.pointSize: 16
                            color: "black"
                        }
                        
                        Rectangle {
                            width: 215
                            height: 70
                            color: "#000000"
                            radius: 5
                            border.color: "#444444"
                            border.width: 2
                            Column {
                                anchors.centerIn: parent
                                spacing: 5

                                Text {
                                    id: measurementValue
                                    text: "0.000 V"
                                    color: "#00FF00"
                                    font.family: "Courier New"
                                    font.pointSize: 16
                                }
                                Text {
                                    id: channelStatus
                                    text: "Статус: idle_state"
                                    color: "#00FF00"
                                    font.family: "Courier New"
                                    font.pointSize: 12
                                }
                            }
                        }

                        Row {                            
                            spacing: 5                            
                            Button {
                                width: 50
                                height: 50
                                text: ""                                
                                icon.source: "qrc:/icon-start.png"                                
                                icon.color: "green"
                                icon.width: 48
                                icon.height: 48
                                enabled: isConnected
                                onClicked: {
                                    let command = "start_measure channel" + modelData;
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateOtherValue(result);                                    
                                }                                
                            }
                            Button {
                                width: 50
                                height: 50
                                text: ""
                                //icon.name: "stop"
                                icon.source: "qrc:/icon-stop.png"
                                icon.color: "red"
                                icon.width: 48
                                icon.height: 48
                                enabled: isConnected
                                onClicked: {
                                    let command = "stop_measure channel" + modelData;
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateOtherValue(result);                                    
                                }
                            }
                            Button {
                                width: 50
                                height: 50
                                text: "?"
                                font.pointSize: 24
                                enabled: isConnected                                
                                onClicked: {
                                    let command = "get_status channel" + modelData;
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateOtherValue(result);                                  
                                }
                            }
                            Button {
                                width: 50
                                height: 50
                                text: "="
                                font.pointSize: 24
                                enabled: isConnected                                
                                onClicked: {                       
                                    let command = "get_result channel" + modelData;
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateMeasurementValue(result);
                                }
                            }
                        }

                        Row {
                            spacing: 5  
                            Button {
                                width: 50
                                height: 50
                                text: "0"
                                icon.source: "qrc:/icon-range.png"    
                                icon.color: "gray"
                                enabled: isConnected
                                onClicked: {
                                    let command = "set_range channel" + modelData + ", 0";
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateOtherValue(result);  
                                }
                            }    
                            Button {
                                width: 50
                                height: 50
                                text: "1"
                                icon.source: "qrc:/icon-range.png"    
                                icon.color: "gray"
                                enabled: isConnected
                                onClicked: {
                                    let command = "set_range channel" + modelData + ", 1";
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateOtherValue(result);  
                                }
                            }
                            Button {
                                width: 50
                                height: 50
                                text: "2"
                                icon.source: "qrc:/icon-range.png"    
                                icon.color: "gray"
                                enabled: isConnected
                                onClicked: {
                                    let command = "set_range channel" + modelData + ", 2";
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateOtherValue(result);  
                                }
                            }
                            Button {
                                width: 50
                                height: 50
                                text: "3"
                                icon.source: "qrc:/icon-range.png"    
                                icon.color: "gray"
                                enabled: isConnected
                                onClicked: {
                                    let command = "set_range channel" + modelData + ", 3";
                                    console.log(command);
                                    let result = client.sendCommand(command);
                                    updateOtherValue(result);  
                                }
                            }                      
                        }
                    }
                }

                function updateMeasurementValue(value) {   
                    if (value === "undefined" || value === null) {                                        
                        // if error
                    } else {                 
                        let values = value.split(", ");
                        if (values[0] == "ok") {
                            measurementValue.text = values[1];
                            channelStatus.text = ""
                        } else {
                            measurementValue.text = values[0];
                            channelStatus.text = values[1]
                        }
                    }
                }

                function updateOtherValue(value) {
                    let values = value.split(", ");
                    measurementValue.text = values[0];
                    if (values.length > 1) {
                        channelStatus.text = values[1];
                    }
                }

            }            
        }
    }
}
