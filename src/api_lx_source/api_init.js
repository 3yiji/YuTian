const lxRequest = require('./lx-request.js'); // 路径要和 lx-request.js 一致
const {lxUtils} = require('./lxTools.ts'); // 引入 lxTools 模块（路径要和 lxTools.js 一致）

// 1. 模拟 globalThis.lx 对象（按 API 规范实现核心方法）
globalThis.lx = {                           // 定义全局 lx 对象
    version: "1.0.0", // 自定义源 API 版本
    env: "desktop", // 运行环境
    currentScriptInfo: {}, // 脚本信息（后续解析）
    EVENT_NAMES: {
        inited: "inited",
        request: "request",
        updateAlert: "updateAlert",
    },
    // 模拟事件注册（工具注册 request 回调）
    on: function (eventName, handler) {
        console.log(`脚本注册事件：${eventName}`);
        globalThis.lx.eventHandlers = globalThis.lx.eventHandlers || {};
        globalThis.lx.eventHandlers[eventName] = handler;
    },
    // 模拟事件发送（脚本发送 inited 事件）
    send: function (eventName, data) {
        console.log(`脚本发送事件：${eventName}，数据：`, data);
        // 脚本发送 inited 后，模拟工具调用 request 事件（获取音乐 URL）
        if (eventName === globalThis.lx.EVENT_NAMES.inited) {
            globalThis.lx.inited_data = data;
            // mockRequest(globalThis.lx.inited_data.sources);
        }
    },
    // 模拟 HTTP 请求（按 API 规范实现）
    request: lxRequest,
    utils: lxUtils,
};
