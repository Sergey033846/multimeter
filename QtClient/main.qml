import QtQuick.Window 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import com.example.client 1.0

ApplicationWindow {
    visible: true
    width: 600
    height: 500
    title: "Client Application"

    Client {
        id: client
        onStatusChanged: statusIndicator.text = status
        onCommandResponse: updateChannelResponse(currentChannel, response)
        
        onUpdateChannelText: {            
            channelText0.text = "Channel0\n" + status;              
            //repeater.itemAt(channelIndex).channelText.text = status;                        
        }
    }

    property int currentChannel: -1

    Column {
        spacing: 20
        anchors.centerIn: parent

        Label {
            id: statusIndicator
            text: "Disconnected"
            color: statusIndicator.text === "Connected" ? "green" : "red"
            font.pixelSize: 18
        }

        Button {
            text: "Connect to Server"
            onClicked: {                
                client.connectToServer()
            }
        }

        Row {
            spacing: 10

            Rectangle {
                id: channelWindow0
                width: 200
                height: 100
                border.color: "black"
                border.width: 2
                color: "white"
                                
                Text {
                    id: channelText0
                    anchors.centerIn: parent
                    text: "Channel0"
                    color: "black"
                }                
            }

            Column {      
                spacing: 10      
                Row {
                    spacing: 10
                    Button {
                        width: 120
                        text: "get_status"
                        onClicked: {                    
                            client.sendCommand("get_status channel0")
                        }
                    }
                    Button {
                        width: 120
                        text: "get result"
                        onClicked: {                    
                            client.sendCommand("get_result channel0")
                        }
                    }
                }

                Row {
                    spacing: 10
                    Button {
                        width: 120
                        text: "start_measure"
                        onClicked: {                    
                            client.sendCommand("start_measure channel0")
                        }
                    }
                    Button {
                        width: 120
                        text: "stop_measure"
                        onClicked: {                    
                            client.sendCommand("stop_measure channel0")
                        }
                    }
                }

                Row {
                    spacing: 10
                    Button {
                        width: 120
                        text: "set_range"
                        onClicked: {                    
                            client.sendCommand("set_range channel0, 2")
                        }
                    }                    
                }
            }
        }

        /*Repeater {
            id: repeater
            model: 3
            Row {
                spacing: 10

                Rectangle {
                    id: channelWindow
                    width: 200
                    height: 100
                    border.color: "black"
                    border.width: 2
                    color: "white"

                    Text {
                        id: channelText
                        anchors.centerIn: parent
                        text: "Channel " + (index) + "\nStatus: Idle"
                        color: "black"
                    }
                }

                Button {
                    text: "Start/Stop"
                    onClicked: {
                        currentChannel = index
                        client.sendCommand(channelText.text.includes("Idle")
                                           ? `start_measure channel${index}`
                                           : `stop_measure channel${index}`)
                    }
                }
            }
        }*/
    }
}
