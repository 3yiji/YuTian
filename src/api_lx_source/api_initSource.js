const path = require('path');           // path = 路径模块
const fs = require('fs');              // fs = File System（文件系统模块

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
