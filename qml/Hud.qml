import QtQuick 2.2

Item {
    id: hud

    property vector3d angle: geometryGrid.angle
    property real zoomFactor: geometryGrid.zoomFactor

    property vector2d xRange: shaderGrid.xRange
    property vector2d yRange: shaderGrid.yRange
    property vector2d zRange: shaderGrid.zRange

    property vector2d huRange: Qt.vector2d(0, 65536)

    property int minHU: shaderGrid.minHU
    property int maxHU: shaderGrid.maxHU

    property real angleShot: 0.0

    Rectangle {
        color: "#71888888"

        anchors.fill: parent

        ShaderGrid {
            id: shaderGrid

            huRange: hud.huRange

            anchors.left: parent.left
        }

        GeometryGrid {
            id: geometryGrid

            angleShot: hud.angleShot

            anchors.right: parent.right
            anchors.rightMargin: 10
        }
    }
}
