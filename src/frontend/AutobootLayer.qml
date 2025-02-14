// Pegasus Frontend
// Copyright (C) 2017-2018  Mátyás Mustoha
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

import QtQuick 2.0

Rectangle {
    id: root

    property real progress: 0
    property bool showDataProgressText: true
    property alias stage: gameCounter.text

    color: "#222"
    anchors.fill: parent

    Image {
        id: logo

        source: "assets/logo.png"
        width: Math.min(parent.width, parent.height)
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignBottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: vpx(-45)
    }

    Rectangle {
        id: progressRoot

        property int padding: vpx(5)

        width: logo.width * 0.94
        height: vpx(30)
        radius: vpx(10)
        color: "#181818"
        anchors.top: logo.bottom
        anchors.topMargin: height * 1
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true

        Image {
            property int animatedWidth: 0

            source: "assets/pbar.png"
            width: parent.width + animatedWidth
            height: parent.height - progressRoot.padding * 2
            fillMode: Image.Tile
            horizontalAlignment: Image.AlignLeft
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: parent.width * (1 - root.progress)

            Image {
                source: "assets/pbar-right.png"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                fillMode: Image.PreserveAspectFit
            }

            SequentialAnimation on animatedWidth {
                loops: Animation.Infinite

                PropertyAnimation {
                    duration: 500
                    to: vpx(68)
                }

                PropertyAnimation {
                    duration: 0
                    to: 0
                }

            }

        }

        Image {
            source: "assets/pbar-left.png"
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            fillMode: Image.PreserveAspectFit
        }

        Image {
            source: "assets/pbar-right.png"
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            fillMode: Image.PreserveAspectFit
        }

        Rectangle {
            // inner border above the image
            anchors.fill: parent
            color: "transparent"
            radius: vpx(10)
            border.width: parent.padding
            border.color: parent.color
        }

    }

    Text {
        id: gameCounter

        visible: showDataProgressText
        color: "#999"
        font.pixelSize: vpx(16)
        font.family: global.fonts.sans
        font.italic: true
        anchors.top: progressRoot.bottom
        anchors.topMargin: vpx(8)
        anchors.right: progressRoot.right
        anchors.rightMargin: vpx(5)
    }

    Behavior on progress {
        NumberAnimation {
        }

    }

}
