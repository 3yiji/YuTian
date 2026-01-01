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
    // 模拟 async 操作
    console.log("开始手动调用脚本获取音乐 URL...");
    const musicUrl = await handler(requestParams);
    console.log("脚本返回的音乐 URL：", musicUrl);
    return musicUrl;
  },
  async getInitedData(){
    return globalThis.lx.inited_data;
  },
  async test(params){
    console.log("test params:", params);
    return "test success";
  }

};

rl.on("line", async (line) => {
  try {
    const msg = JSON.parse(line);
    const { id, method, params } = msg;

    if (!handlers[method]) {
      throw new Error("unknown method");
    }

    const result = await handlers[method](params);

    process.stdout.write(JSON.stringify({
      id,
      result
    }) + "\n");

  } 
  catch (e) {
    process.stdout.write(JSON.stringify({
      error: e.message
    }) + "\n");
  }
});
