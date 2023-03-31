// Pegasus Frontend
// Copyright (C) 2017  Mátyás Mustoha
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

import QtQuick 2.7
import QtQuick.Controls 2.7

Item {
    id: root

    property int value: 5
    property alias pressed: slider.pressed

    width: height * 2
    Keys.onPressed: {
        if (api.keys.isLeft(event)) {
            event.accepted = true;
            slider.decrease();
            value = slider.value;
        }
        if (api.keys.isRight(event)) {
            event.accepted = true;
            slider.increase();
            value = slider.value;
        }
    }

    Slider {
        id: slider

        value: root.value
        to: 10
        snapMode: Slider.SnapAlways
        stepSize: 1
        anchors.right: parent.right
        anchors.rightMargin: parent.rightMargin
        anchors.verticalCenter: parent.verticalCenter

        background: Rectangle {
            x: slider.leftPadding
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            implicitWidth: 200
            implicitHeight: height * 0.5
            width: slider.availableWidth
            height: root.height * 0.5
            radius: height * 0.5
            color: "#bbb"
            opacity: 0.3

            Rectangle {
                width: slider.visualPosition * parent.width
                height: parent.height
                color: "#3aa"
                radius: parent.height * 0.5
            }
        }

        handle: Rectangle {
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: root.height
            height: root.height
            radius: height * 0.5
            implicitWidth: width
            implicitHeight: height
            color: slider.pressed ? "#3aa" : "#bbb"
            border.color: slider.pressed ? "#3aa" : "#bbb"
        }

    }

}
