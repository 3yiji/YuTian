// Generated from SVG file collect.svg
import QtQuick
import QtQuick.VectorImage
import QtQuick.VectorImage.Helpers
import QtQuick.Shapes

Item {
    implicitWidth: 32
    implicitHeight: 32
    component AnimationsInfo : QtObject
    {
        property bool paused: false
        property int loops: 1
        signal restart()
    }
    property AnimationsInfo animations : AnimationsInfo {}
    transform: [
        Scale { xScale: width / 32; yScale: height / 32 }
    ]
    id: __qt_toplevel
    Shape {
        id: _qt_node0
        ShapePath {
            id: _qt_shapePath_0
            strokeColor: "transparent"
            fillColor: "#ffffffff"
            fillRule: ShapePath.WindingFill
            PathSvg { path: "M 0 0 L 32 0 L 32 32 L 0 32 L 0 0 " }
        }
        ShapePath {
            id: _qt_shapePath_1
            strokeColor: "#ff00bfff"
            strokeWidth: 2
            capStyle: ShapePath.FlatCap
            joinStyle: ShapePath.MiterJoin
            miterLimit: 4
            fillColor: "#00000000"
            fillRule: ShapePath.WindingFill
            PathSvg { path: "M 16 8 L 17.7961 13.5279 L 23.6085 13.5279 L 18.9062 16.9443 L 20.7023 22.4721 L 16 19.0557 L 11.2977 22.4721 L 13.0938 16.9443 L 8.39155 13.5279 L 14.2039 13.5279 L 16 8 " }
        }
    }
}
