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

import "qrc:/qmlutils" as PegasusUtils
import QtQuick 2.8
import QtMultimedia 5.9
import QtQuick.Window 2.2

Window {
    id: appWindow

    function vpx(value) {
        return global.winScale * value;
    }

    visible: true
    width: 1280
    height: 720
    title: "FrontLoader"
    color: "#000"
    visibility: Internal.settings.fullscreen ? Window.FullScreen : Window.AutomaticVisibility
    onClosing: {
        theme.source = "";
        Internal.system.quit();
    }

    FontLoader {
        id: sansFont

        source: "/fonts/Roboto-Regular.ttf"
    }

    FontLoader {
        id: sansBoldFont

        source: "/fonts/Roboto-Bold.ttf"
    }

    FontLoader {
        id: monoFont

        source: "/fonts/RobotoMono-Regular.ttf"
    }

    FontLoader {
        id: condensedFont

        source: "/fonts/RobotoCondensed-Regular.ttf"
    }

    FontLoader {
        id: condensedBoldFont

        source: "/fonts/RobotoCondensed-Bold.ttf"
    }

    SoundEffect {
        id: sfxVolume

        source: "assets/volume.wav"
    }

    // a globally avalable utility object
    QtObject {
        id: global

        readonly property real winScale: Math.min(width / 1280, height / 720)
        property QtObject fonts

        fonts: QtObject {
            readonly property string sans: sansFont.name
            readonly property string sansBold: sansBoldFont.name
            readonly property string condensed: condensedFont.name
            readonly property string condensedBold: condensedBoldFont.name
            readonly property string mono: monoFont.name
        }

    }

    // legacy global objects
    QtObject {
        id: globalFonts

        readonly property string sans: global.fonts.sans
        readonly property string condensed: global.fonts.condensed
    }

    // the main content
    FocusScope {
        id: content

        anchors.fill: parent
        enabled: focus

        Loader {
            id: theme

            readonly property url apiThemePath: Internal.settings.themes.currentQmlPath

            function getThemeFile() {
                if (Internal.scanner.running || Internal.autoboot.running)
                    return "";

                if (api.collections.count === 0)
                    return "messages/NoGamesError.qml";

                return apiThemePath;
            }

            anchors.fill: parent
            focus: true
            enabled: focus
            onApiThemePathChanged: source = Qt.binding(getThemeFile)
            Keys.onPressed: {
                if (api.keys.isCancel(event) || api.keys.isMenu(event)) {
                    event.accepted = true;
                    mainMenu.focus = true;
                }
                if (api.keys.isVolUp(event)) {
                    event.accepted = true;
                    Internal.system.volumeUp();
                    sfxVolume.play();
                }
                if (api.keys.isVolDown(event)) {
                    event.accepted = true;
                    Internal.system.volumeDown();
                    sfxVolume.play();
                }
                if (event.key === Qt.Key_F5) {
                    event.accepted = true;
                    theme.source = "";
                    Internal.meta.clearQMLCache();
                    theme.source = Qt.binding(getThemeFile);
                }
            }
            source: getThemeFile()
            asynchronous: true
            onStatusChanged: {
                if (status == Loader.Error)
                    source = "messages/ThemeError.qml";

            }
            onLoaded: item.focus = focus
            onFocusChanged: {
                if (item)
                    item.focus = focus;

            }
        }

        Loader {
            id: mainMenu

            anchors.fill: parent
            source: "MenuLayer.qml"
            asynchronous: true
            onLoaded: item.focus = focus
            onFocusChanged: {
                if (item)
                    item.focus = focus;

            }
            enabled: focus
        }

        Connections {
            function onClose() {
                theme.focus = true;
            }

            function onRequestShutdown() {
                powerDialog.source = "dialogs/ShutdownDialog.qml";
                powerDialog.focus = true;
            }

            function onRequestSuspend() {
                Internal.system.suspend();
            }

            function onRequestReboot() {
                powerDialog.source = "dialogs/RebootDialog.qml";
                powerDialog.focus = true;
            }

            function onRequestQuit() {
                theme.source = "";
                Internal.system.quit();
            }

            target: mainMenu.item
        }

        PegasusUtils.HorizontalSwipeArea {
            id: menuSwipe

            width: vpx(40)
            height: parent.height
            anchors.right: parent.right
            onSwipeLeft: {
                if (!mainMenu.focus)
                    mainMenu.focus = true;

            }
        }

    }

    Loader {
        id: powerDialog

        anchors.fill: parent
    }

    Connections {
        function onCancel() {
            content.focus = true;
        }

        target: powerDialog.item
    }

    Loader {
        id: multifileSelector

        anchors.fill: parent
    }

    Connections {
        function onCancel() {
            content.focus = true;
        }

        target: multifileSelector.item
    }

    Loader {
        id: genericMessage

        anchors.fill: parent
    }

    Connections {
        function onClose() {
            content.focus = true;
        }

        target: genericMessage.item
    }

    Connections {
        function onEventSelectGameFile(game) {
            multifileSelector.setSource("dialogs/MultifileSelector.qml", {
                "game": game
            });
            multifileSelector.focus = true;
        }

        function onEventLaunchError(msg) {
            genericMessage.setSource("dialogs/GenericOkDialog.qml", {
                "title": qsTr("Error"),
                "message": msg
            });
            genericMessage.focus = true;
        }

        target: api
    }

    Connections {
        function onRunningChanged() {
            if (Internal.scanner.running)
                splashScreen.focus = true;
        }

        target: Internal.scanner
    }

    Connections {
        function onRunningChanged() {
            if (Internal.autoboot.running)
                autobootScreen.focus = true;
        }

        target: Internal.autoboot
    }

    SplashLayer {
        id: splashScreen

        readonly property bool dataLoading: Internal.scanner.running
        readonly property bool skinLoading: theme.status === Loader.Null || theme.status === Loader.Loading

        function hideMaybe() {
            if (focus && !dataLoading && !skinLoading) {
                content.focus = true;
                Internal.scanner.reset();
            }
        }

        focus: true
        enabled: false
        visible: focus
        showDataProgressText: dataLoading
        progress: Internal.scanner.progress
        stage: Internal.scanner.stage
        onSkinLoadingChanged: hideMaybe()
        onDataLoadingChanged: hideMaybe()
    }

    AutobootLayer {
        id: autobootScreen

        readonly property bool dataLoading: Internal.autoboot.running
        readonly property bool skinLoading: theme.status === Loader.Null || theme.status === Loader.Loading

        function hideMaybeAutoboot() {
            if (focus && !dataLoading && !skinLoading) {
                content.focus = true;
                Internal.autoboot.reset();
            }
        }

        Keys.onPressed: {
            if (api.keys.isCancel(event) || api.keys.isAccept(event)) {
                event.accepted = true;
                Internal.autoboot.cancel();
            }
        }

        focus: false
        enabled: true
        visible: focus
        showDataProgressText: dataLoading
        progress: Internal.autoboot.progress
        stage: Internal.autoboot.stage
        onSkinLoadingChanged: hideMaybeAutoboot()
        onDataLoadingChanged: hideMaybeAutoboot()
    }

}
