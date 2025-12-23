// common/Theme.qml
pragma Singleton  // 标记为单例
import QtQuick 2.15

// 关键：定义单例组件（Qt.createQmlObject 或 qmldir 声明，这里用更简单的 qmldir 方式）


Item {
    // 1. 主色调（对应你的侧边栏、选中态等）
    property color primary: "#27be8c"          // 主绿（原侧边栏选中色）
    property color primaryDark: "#38a422"      // 深绿（原侧边栏子项色）
    property color primaryLight: "#CBE8DA"     // 浅绿（原选中高亮色）

    // 2. 辅助色（按钮、提示等）
    property color accent: "#e0bf1a"           // 黄色（原悬停色）
    property color danger: "#de1010"           // 红色（错误/删除）
    property color purple: "#972ad1"           // 紫色（原侧边栏背景）

    // 3. 中性色（文本、背景、边框等）
    property color background: "#F3FBF8"       // 页面背景色
    property color textPrimary: "#333333"      // 主要文本色
    property color textSecondary: "#666666"    // 次要文本色
    property color border: "#EEEEEE"           // 边框色
    property color transparent: "transparent"  // 透明色

    // 4. 可选：主题模式（亮色/暗色），支持动态切换
    property bool darkMode: false

    // 动态主题逻辑（darkMode 切换时，自动更新颜色）
    onDarkModeChanged: {
        if (darkMode) {
            // 暗色模式颜色
            primary = "#1a9c6e"
            primaryDark = "#2d8b1e"
            background = "#1E1E1E"
            textPrimary = "#FFFFFF"
            textSecondary = "#BBBBBB"
        } else {
            // 亮色模式颜色（恢复默认）
            primary = "#27be8c"
            primaryDark = "#38a422"
            background = "#F3FBF8"
            textPrimary = "#333333"
            textSecondary = "#666666"
        }
    }
}