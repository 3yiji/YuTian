// mock-tool.js：模拟工具的 API 环境，调用脚本
const fs = require('fs');               // fs = File System（文件系统模块
const path = require('path');           // path = 路径模块
const crypto = require('crypto');       // crypto = 加密模块
const zlib = require('zlib');           // zlib = 压缩模块
const { promisify } = require('util');  // promisify = 将回调函数转为 Promise
const lxRequest = require('./lx-request.js'); // 路径要和 lx-request.js 一致

const {lxUtils} = require('./lxTools.ts'); // 引入 lxTools 模块（路径要和 lxTools.js 一致）
// import lxTools from './lxTools.ts';
// 将 zlib 的回调函数转为 Promise
const inflateAsync = promisify(zlib.inflate);
const deflateAsync = promisify(zlib.deflate);

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
    // 模拟工具方法（按 API 规范实现）
    // utils: {
    //     buffer: {
    //         from: (data) => Buffer.from(data),
    //         bufToString: (buf, format) => buf.toString(format),
    //     },
    //     crypto: {
    //         // MD5 加密
    //         md5: (str) => crypto.createHash('md5').update(str).digest('hex'),
            
    //         // AES 加密 aesEncrypt(buffer, mode, key, iv)
    //         // mode: 加密模式，如 'aes-128-cbc', 'aes-256-cbc' 等
    //         aesEncrypt: (buffer, mode, key, iv) => {
    //             const cipher = crypto.createCipheriv(mode, key, iv);
    //             return Buffer.concat([cipher.update(buffer), cipher.final()]);
    //         },
            
    //         // 生成随机字节
    //         randomBytes: (size) => crypto.randomBytes(size),
            
    //         // RSA 加密 rsaEncrypt(buffer, key)
    //         // key: PEM 格式的公钥字符串
    //         rsaEncrypt: (buffer, key) => {
    //             return crypto.publicEncrypt(key, buffer);
    //         },
    //     },
    //     zlib: {
    //         // 解压 inflate(buffer: Buffer) => Promise<Buffer>
    //         inflate: (buf) => inflateAsync(buf),
            
    //         // 压缩 deflate(buffer: Buffer) => Promise<Buffer>
    //         deflate: (buf) => deflateAsync(buf),
    //     },
    // },
    utils: lxUtils,
};

// 2. 模拟工具调用 request 事件（触发脚本获取音乐 URL）
async function mockRequest(sources) {
  // 选择脚本支持的源（如 sources 中的 mg 键）
  const sourceKey = Object.keys(sources)[4]; // 如 "mg"
  if (!sourceKey) {
    console.log("脚本未声明支持的源");
    return;
  }

  // 模拟工具传递的参数（请求 flac 音质的音乐 URL）
  const requestParams = {
    source: sourceKey,
    action: "musicUrl", // 脚本支持的 action
    info: {
      type: sources[sourceKey].qualitys[0], // 脚本支持的音质（从 sources[sourceKey].qualitys 中选）
      musicInfo: {
        // id: "123456", // 模拟歌曲 ID（需替换为脚本支持的真实 ID）
        name: "成都",
        singer: "赵雷",
        songmid: "1106531626",
        hash: "a06b033b356bfc974c5245d0195086a5",
        albumId: 1802652,
        id: 1106531626,
      },
    },
  };

  // 调用脚本注册的 request 回调
  console.log("globalThis.lx.EVENT_NAMES:", globalThis.lx.EVENT_NAMES);
  const handler = globalThis.lx.eventHandlers[globalThis.lx.EVENT_NAMES.request];
  if (!handler) {
    console.log("脚本未注册 request 回调");
    return;
  }

  try {
    console.log("开始手动调用脚本获取音乐 URL...");
    const musicUrl = await handler(requestParams);
    console.log("脚本返回的音乐 URL：", musicUrl);
    console.log("手动调用成功！复制 URL 到播放器即可播放");
  } catch (err) {
    console.log("手动调用失败：", err.message);
  }
}

// 3. 加载并执行脚本
const scriptPath = path.join(__dirname, "lx-music-source.js");
const scriptCode = fs.readFileSync(scriptPath, "utf8");

// 解析脚本开头的注释（填充 currentScriptInfo）
const commentMatch = scriptCode.match(/\/\*\*[\s\S]*?\*\//);
if (commentMatch) {
  const comment = commentMatch[0];
  globalThis.lx.currentScriptInfo.name = comment.match(/@name\s+(.+)/)?.[1] || "";
  globalThis.lx.currentScriptInfo.description = comment.match(/@description\s+(.+)/)?.[1] || "";
}

// 执行脚本（触发初始化和事件）
// eval(scriptCode);
require(scriptPath); // 使用 require 执行脚本

console.log(`globalThis: ${JSON.stringify(globalThis.lx)}`);  // 直接打印对象
console.log(`globalThis.lx.on: ${globalThis.lx.on.toString()}`);  // 打印函数

