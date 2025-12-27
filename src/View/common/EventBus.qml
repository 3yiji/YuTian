pragma Singleton  // 标记为单例
import QtQuick 2.15

QtObject{
    // 事件总线：定义全局信号，供各组件通信使用
    signal searchRequested(string keyword)   // 搜索请求信号
}