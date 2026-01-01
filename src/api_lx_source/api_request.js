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

mockRequest(globalThis.lx.inited_data.sources);
console.log("模拟工具请求完成");