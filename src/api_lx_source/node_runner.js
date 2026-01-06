// node_runner.js
import readline from "readline";

import './api_init.js'; // 初始化 globalThis.lx 对象
import './api_initSource.js'; // 加载并执行脚本

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

const handlers = {
    async request(requestParams) {
        console.log("globalThis.lx.EVENT_NAMES:", globalThis.lx.EVENT_NAMES);
        const handler = globalThis.lx.eventHandlers[globalThis.lx.EVENT_NAMES.request];
        if (!handler) {
            console.log("脚本未注册 request 回调");
            return;
        }

        let result;
        try {
            const musicUrl = await handler(requestParams);
            result = {
                success: true,
                msg: '获取url成功',
                data: musicUrl // 透传原始数据
            };
        } catch (err) {
            console.log("手动调用失败：", err.message);
            result = {
                success: false,
                msg: err.message,
                data: null
            };
        }
        return result;
    },

    async initSource(scriptCode){
        let isInitResultSent = false;

        // 用 Promise 封装异步回调结果
        const initPromise = new Promise((resolve, reject) => {
            // 1. 重写 globalThis.lx.send 监听初始化事件
            globalThis.lx.send = function(eventName, data) {
                // 仅处理初始化完成事件，且只返回一次结果
                if (eventName === globalThis.lx.EVENT_NAMES.inited && !isInitResultSent) {
                    isInitResultSent = true;
                    
                    // 解析洛雪音源返回的初始化结果
                    const success = data.status;
                    let result;
                    if (success){
                        result = {
                            success: true,
                            msg: '脚本初始化成功',
                            data: data // 透传原始数据
                        };
                    }
                    else{
                        result = {
                            success: false,
                            msg: '脚本初始化失败',
                            data: null // 透传原始数据
                        };
                    }
                    
                    // 成功/失败都 resolve（用 success 字段区分，避免 reject 捕获异常）
                    resolve(result);
                }
            };

            // 2. 执行传入的脚本代码（核心初始化步骤）
            try {
                eval(scriptCode); // 执行洛雪音源脚本
            } 
            catch (evalErr) {
                // 脚本执行出错，直接返回失败结果
                isInitResultSent = true;
                reject({
                    success: false,
                    msg: `脚本执行异常：${evalErr.message}`,
                    data: evalErr
                });
            }

            // 3. 超时兜底：5秒未触发 inited 事件则判定为失败
            setTimeout(() => {
                if (!isInitResultSent) {
                    isInitResultSent = true;
                    reject({
                        success: false,
                        msg: '脚本初始化超时：未触发 inited 事件',
                        data: null
                    });
                }
            }, 5000); // 5秒超时，可根据需要调整
        });

        // 4. 等待 Promise 结果并返回（实现异步转同步返回）
        try {
            const initResult = await initPromise;
            return initResult; // 最终返回初始化结果
        } catch (err) {
            return err; // 捕获超时/执行异常，返回失败结果
        }
    },

    async test(params){
        console.log("test params:", params);
        return "test success";
    }

};

rl.on("line", async (line) => {
  try {
    const msg = JSON.parse(line);
    const { id, body} = msg;
    const {method, params} = body;

    if (!handlers[method]) {
      throw new Error("unknown method");
    }

    const result = await handlers[method](params);

    process.stdout.write(JSON.stringify({
      id,
      responseBody: result
    }) + "\n");

  } 
  catch (e) {
    process.stdout.write(JSON.stringify({
      error: e.message
    }) + "\n");
  }
});

process.stdout.write(JSON.stringify({
      id: "Global_Message",
      responseBody: {msg: "Node.js process is ready"}
}) + "\n");
