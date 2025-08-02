import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CitySearcher

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: "City Searcher"

    CitySearchViewModel {
        id: viewModel
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // Title
        Text {
            text: qsTr("City Search")
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        // Search Section
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Enter city name...")
                enabled: !viewModel.isSearching
                
                Keys.onReturnPressed: searchButton.clicked()
                Keys.onEnterPressed: searchButton.clicked()
            }

            Button {
                id: searchButton
                text: viewModel.isSearching ? qsTr("Searching...") : qsTr("Search")
                enabled: !viewModel.isSearching && searchField.text.trim().length > 0
                
                onClicked: {
                    viewModel.searchCities(searchField.text)
                }
            }

            Button {
                text: qsTr("Clear")
                enabled: !viewModel.isSearching
                onClicked: {
                    searchField.text = ""
                    viewModel.clearResults()
                }
            }
        }

        // Loading indicator
        ProgressBar {
            Layout.fillWidth: true
            visible: viewModel.isSearching
            indeterminate: true
        }

        // Error message
        Text {
            Layout.fillWidth: true
            text: viewModel.errorMessage
            color: "red"
            visible: viewModel.errorMessage.length > 0
            wrapMode: Text.WordWrap
        }

        // Results count
        Text {
            Layout.fillWidth: true
            text: qsTr("Found %1 cities").arg(viewModel.cityListModel.count)
            visible: viewModel.cityListModel.count > 0 && !viewModel.isSearching
            color: "darkgreen"
        }

        // Results List
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: cityListView
                model: viewModel.cityListModel
                
                delegate: ItemDelegate {
                    width: cityListView.width
                    height: 80
                    
                    onClicked: {
                        console.log("Clicked on city:", model.name, "at coordinates:", model.latitude, model.longitude)
                        viewModel.openCityInBrowser(model.latitude, model.longitude, model.name)
                    }
                    
                    Rectangle {
                        anchors.fill: parent
                        color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f0f0f0" : "transparent")
                        border.color: "#e0e0e0"
                        border.width: 1
                        radius: 4
                        
                        ColumnLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.margins: 10
                            spacing: 4
                            
                            Text {
                                text: model.name
                                font.bold: true
                                font.pixelSize: 16
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                            
                            Text {
                                text: model.displayName
                                color: "gray"
                                font.pixelSize: 12
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                wrapMode: Text.WordWrap
                                maximumLineCount: 2
                            }
                            
                            Text {
                                text: qsTr("Coordinates: %1, %2 • Click to open in map").arg(model.latitude.toFixed(4)).arg(model.longitude.toFixed(4))
                                color: "darkblue"
                                font.pixelSize: 10
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
                
                // Empty state
                Text {
                    anchors.centerIn: parent
                    text: viewModel.isSearching ? "" : qsTr("No cities found. Try searching for a city name.")
                    color: "gray"
                    visible: cityListView.count === 0 && !viewModel.isSearching
                }
            }
        }
    }

    // Focus on search field when window opens
    Component.onCompleted: {
        searchField.forceActiveFocus()
    }
}
