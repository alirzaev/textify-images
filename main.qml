import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

ApplicationWindow {
    width: 500
    height: 350
    visible: true
    title: qsTr("Processing photos with text")

    GridLayout {
        enabled: !_backend.busy
        anchors {
            fill: parent
            margins: 10
        }

        rows: 2
        columns: 2
        rowSpacing: 10
        columnSpacing: 10

        ListView {
            clip: true
            model: _backend.images
            spacing: 5
            delegate: ItemDelegate {
                width: parent.width
                text: model.modelData
            }
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollBar.vertical: ScrollBar { }
        }

        ColumnLayout {
            spacing: 10
            Layout.fillHeight: true
            Layout.maximumWidth: 120
            Layout.alignment: Qt.AlignTop

            Button {
                text: qsTr("Select...")
                onClicked: _backend.selectImages()
                Layout.fillWidth: true
            }

            CheckBox {
                text: qsTr("Grayscaled")
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Process")
                onClicked: {
                    _backend.convertImages()
                }
                Layout.fillWidth: true
            }
        }

        ProgressBar  {
            from: 0
            to: 1
            value: _backend.progress
            Layout.row: 1
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }
    }

    MessageDialog {
        id: convertionDoneDialog
        title: qsTr("Processing")
        text: qsTr("Done!")
        visible: false
        onAccepted: {
            close();
        }

        Connections {
            target: _backend
            function onConvertionDone() {
                convertionDoneDialog.open()
            }
        }
    }
}
