import QtQuick 2.2

import TomographyTools 1.0

Item {
    id: sliceItem

    property bool takeShot: false

    Rectangle {

        Hud {
            id: sliceHud
            objectName: "sliceHud"

            width: sliceItem.width
            height: sliceItem.height * 0.35

            huRange: sliceViewer.huRange

            visible: false
        }

        Rectangle {
            id: horizontalSeparator

            color: "#FF888888"
            anchors.top: sliceHud.top
            x: sliceItem.width / 2 - 5
            width: 10
            height: sliceItem.height

            visible: false

            z: -1
        }

        Rectangle {
            id: verticalSeparator

            color: "#FF888888"
            y: sliceItem.height / 2 - 5
            height: 10
            width: sliceItem.width

            visible: false

            z: -1
        }

        SliceViewer {
            id: sliceViewer
            objectName: "sliceViewer"

            width: sliceItem.width
            height: sliceItem.height
            z: -2

            sRange: sliceHud.sRange
            tRange: sliceHud.tRange
            pRange: sliceHud.pRange

            rotation: sliceHud.angle

            zoomFactor: sliceHud.zoomFactor

            minHU: sliceHud.minHU
            maxHU: sliceHud.maxHU

            takeShot: sliceItem.takeShot
        }
    }

    onTakeShotChanged: {
        if (sliceItem.takeShot) {
            angleShotTimer.running = true;
        }
    }

    function show() {
        sliceHud.visible = !sliceHud.visible;
        horizontalSeparator.visible = !horizontalSeparator.visible;
        verticalSeparator.visible = !verticalSeparator.visible;
    }

    Timer {
        id: angleShotTimer
        interval: 200

        repeat: true
        running: false

        onTriggered: {
            sliceHud.angleShot += 1.0;

            if (sliceHud.angleShot > 180.0) {
                angleShotTimer.running = false;
                sliceHud.angleShot = 0.0;

                sliceItem.takeShot = false;
            }
        }
    }
}
