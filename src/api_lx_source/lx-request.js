// lx-request.js（独立文件，带完整调试日志）
const fetch = require('node-fetch');
const FormData = require('form-data');
const { URLSearchParams } = require('url');
// const { callbackify } = require('util');

// 导出 HTTP 请求实现函数
function lxRequest(url, options = {}, callback) {
    
    // 调试日志：请求发起（标记请求唯一ID，方便多请求时区分）
    const requestId = `[REQ-${Date.now().toString().slice(-6)}]`; // 6位时间戳作为请求ID

    
    // //   console.log('\n==================================================');
    console.log(`\n${"-".repeat(50)}`);
    console.log(`\n${requestId} 脚本请求 URL：${url}，参数：`, options);
    
    // 1. 初始化默认参数
    const method = (options.method || 'GET').toUpperCase();
    const headers = options.headers || {};
    const timeout = options.timeout || 10000;           //如果 options.timeout 有值，就用它；如果没有值，就用 10000
    let body = null;

    // 2. 处理请求体（form/formData/body）
    if (method !== 'GET' && method !== 'HEAD') {
        if (options.formData) {
            body = new FormData();
            Object.entries(options.formData).forEach(([key, value]) => {
                body.append(key, value);
            });
            Object.assign(headers, body.getHeaders());
            // 调试日志：formData 请求体
            console.log(`${requestId} 请求体（formData）:`);
            Object.entries(options.formData).forEach(([key, value]) => {
                console.log(`  ${key}: ${typeof value === 'object' ? '[文件/二进制]' : value}`);
            });
        } 
        else if (options.form) {
            body = new URLSearchParams(options.form);
            headers['Content-Type'] = headers['Content-Type'] || 'application/x-www-form-urlencoded';
            // 调试日志：form 请求体（格式化）
            console.log(`${requestId} 请求体（form）:`);
            Object.entries(options.form).forEach(([key, value]) => {
                console.log(`  ${key}: ${value}`);
            });
        } 
        else if (options.body) {
            if (typeof options.body === 'object' && !Buffer.isBuffer(options.body)) {
                body = JSON.stringify(options.body);
                headers['Content-Type'] = headers['Content-Type'] || 'application/json';
                // 调试日志：JSON 请求体（格式化输出）
                console.log(`${requestId} 请求体（JSON）:`);
                console.log(`  ${JSON.stringify(options.body, null, 2)}`);
            } else {
                body = options.body;
                // 调试日志：普通文本/Buffer 请求体
                console.log(`${requestId} 请求体（${Buffer.isBuffer(body) ? 'Buffer' : '文本'}）:`);
                console.log(`  ${Buffer.isBuffer(body) ? `[Buffer长度: ${body.length}字节]` : body}`);
            }
        } 
        else {
            console.log(`${requestId} 无请求体`);
        }
    } 
    else {
        console.log(`${requestId} 无请求体（GET/HEAD 方法）`);
    }

    // 3. 构造 fetch 配置
    const fetchOptions = {
        method,
        headers,
        body,
        timeout,
        redirect: 'follow',
        compress: true,
    };

    // 4. 支持取消请求
    let abortController = null;
    if (fetch.AbortController) {
        abortController = new AbortController();
        fetchOptions.signal = abortController.signal;
    }

    // 5. 发送请求并处理响应
    console.log(`${requestId} 实际发送请求的配置`);
    console.log(`${requestId} url: ${url}`);
    console.log(`${requestId} fetchOptions：`, fetchOptions);
    let requestPromise = fetch(url, fetchOptions)
        .then(async (resp) => {
            // 调试日志：响应状态
            console.log(`${requestId} 响应状态: ${resp.status} ${resp.statusText}`);

            // 调试日志：响应头（格式化输出）
            console.log(`${requestId} 响应头:`);
            const responseHeaders = {};
            resp.headers.forEach((value, key) => {
                responseHeaders[key] = value;
                // console.log(`  ${key}: ${value}`);
            });

            // 构造响应对象
            const response = {
                statusCode: resp.status,
                statusMessage: resp.statusText,
                headers: responseHeaders,
            };

            // 处理响应体
            const contentType = resp.headers.get('content-type') || '';
            let responseBody = null;

            if (contentType.includes('application/json') || contentType.includes('text/')) {
                responseBody = await resp.json().catch((err) => {
                    console.log(`${requestId} JSON 解析失败: ${err.message}`);
                    return null;
                });
                // 调试日志：JSON 响应体（格式化）
                console.log(`${requestId} 响应体（JSON）:`);
                // console.log(responseBody ? JSON.stringify(responseBody, null, 2) : '解析失败/空响应');
            } 
            else if (contentType.includes('image/') || contentType.includes('application/octet-stream')) {
                const arrayBuffer = await resp.arrayBuffer();
                responseBody = Buffer.from(arrayBuffer);
                // 调试日志：二进制响应体
                console.log(`${requestId} 响应体（二进制）:`);
                console.log(`  类型: ${contentType}`);
                console.log(`  大小: ${responseBody.length} 字节`);
            } 
    
            else {
                responseBody = await resp.text().catch((err) => {
                    console.log(`${requestId} 文本解析失败: ${err.message}`);
                    return null;
                });
                // 调试日志：文本响应体
                console.log(`${requestId} 响应体（文本）:`);
                console.log(responseBody || '解析失败/空响应');
            }

            console.log(`${requestId} 请求成功完成`);
            console.log('==================================================\n');

            // 调用回调返回结果
            // callback(null, response, responseBody);
            response.body = responseBody;
            callback(null, response);
        })
        .catch((err) => {
            // 调试日志：请求错误详情
            let errorType = err.name || 'UnknownError';
            let errorMessage = err.message;

            if (err.name === 'AbortError') {
                errorType = 'RequestAborted';
                errorMessage = '请求被取消';
            } else if (err.type === 'request-timeout') {
                errorType = 'RequestTimeout';
                errorMessage = `请求超时（${timeout}ms）`;
            } else if (err.code === 'ENOTFOUND') {
                errorType = 'DNSNotFound';
                errorMessage = `DNS 解析失败: ${err.hostname}`;
            } else if (err.code === 'ECONNREFUSED') {
                errorType = 'ConnectionRefused';
                errorMessage = `连接被拒绝: ${url}`;
            }

            console.log('\n==================================================');
            console.log(`${requestId} 请求失败`);
            console.log(`${requestId} 错误类型: ${errorType}`);
            console.log(`${requestId} 错误信息: ${errorMessage}`);
            if (err.stack) {
                console.log(`${requestId} 错误堆栈:`);
                console.log(err.stack.slice(0, 500)); // 只输出前500字符，避免日志过长
            }
            console.log('==================================================\n');

            // 调用回调返回错误
            callback(new Error(`${errorType}: ${errorMessage}`), null, null);
        });



    // 6. 返回取消请求方法
    return function cancelRequest() {
        if (abortController) {
            abortController.abort();
            console.log(`${requestId} 已取消请求`);
        }
    };
}

module.exports = lxRequest;