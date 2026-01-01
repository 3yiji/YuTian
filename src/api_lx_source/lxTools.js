"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g = Object.create((typeof Iterator === "function" ? Iterator : Object).prototype);
    return g.next = verb(0), g["throw"] = verb(1), g["return"] = verb(2), typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (g && (g = 0, op[0] && (_ = 0)), _) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.lxUtils = void 0;
/**
 * 实现 globalThis.lx.utils 工具接口
 * 兼容 Node.js 核心 API：Buffer、加密、压缩
 */
var crypto_1 = require("crypto");
var zlib_1 = require("zlib");
var util_1 = require("util");
//  promisify zlib 异步方法（适配 Promise 语法）
var inflateAsync = (0, util_1.promisify)(zlib_1.default.inflate);
var deflateAsync = (0, util_1.promisify)(zlib_1.default.deflate);
/**
 * Buffer 工具类：对应 Node.js Buffer
 */
var buffer = {
    /**
     * 从数据创建 Buffer（兼容 Node.js Buffer.from）
     * @param data 输入数据（字符串、数组、ArrayBuffer 等）
     * @param encoding 编码格式（可选，如 'utf8'、'base64'）
     * @returns Buffer 实例
     */
    from: function (data, encoding) {
        if (typeof data === 'string') {
            return Buffer.from(data, encoding);
        }
        if (data instanceof ArrayBuffer) {
            return Buffer.from(new Uint8Array(data));
        }
        return Buffer.from(data);
    },
    /**
     * Buffer 转字符串（对应 Node.js Buffer.toString）
     * @param buffer 要转换的 Buffer
     * @param format 编码格式（如 'utf8'、'base64'、'hex'，默认 'utf8'）
     * @returns 转换后的字符串
     */
    bufToString: function (buffer, format) {
        if (format === void 0) { format = 'utf8'; }
        if (!Buffer.isBuffer(buffer))
            throw new TypeError('bufToString 参数必须是 Buffer 类型');
        return buffer.toString(format);
    },
};
/**
 * 加密工具类：AES、MD5、RSA、随机字符串
 */
var cryptoTools = {
    /**
     * AES 加密（支持字符串/Buffer 类型输入：数据、密钥、IV）
     * @param buffer 待加密的数据（字符串或 Buffer，默认 utf8 编码）
     * @param mode 加密模式（如 'aes-128-cbc'、'aes-256-gcm'，需包含密钥长度和模式）
     * @param key 加密密钥（字符串或 Buffer，长度需匹配模式：128位=16字节，256位=32字节，默认 utf8 编码）
     * @param iv 初始化向量（字符串、Buffer 或 undefined，CBC 模式必需，默认 utf8 编码）
     * @param encoding 字符串编码（可选，默认 'utf8'，支持 'gbk'、'base64' 等）
     * @returns 加密后的 Buffer
     */
    aesEncrypt: function (buffer, mode, key, iv, encoding // 统一字符串编码参数（数据、key、iv 共用）
    ) {
        if (encoding === void 0) { encoding = 'utf8'; }
        // 1. 数据（buffer）适配：字符串 → Buffer
        var dataBuffer;
        if (typeof buffer === 'string') {
            dataBuffer = Buffer.from(buffer, encoding);
        }
        else if (Buffer.isBuffer(buffer)) {
            dataBuffer = buffer;
        }
        else {
            throw new TypeError('aesEncrypt: 数据必须是字符串或 Buffer 类型');
        }
        // 2. 密钥（key）适配：字符串 → Buffer
        var keyBuffer;
        if (typeof key === 'string') {
            keyBuffer = Buffer.from(key, encoding);
        }
        else if (Buffer.isBuffer(key)) {
            keyBuffer = key;
        }
        else {
            throw new TypeError('aesEncrypt: 密钥必须是字符串或 Buffer 类型');
        }
        // 3. 初始化向量（iv）适配：字符串 → Buffer（支持 undefined）
        var ivBuffer = Buffer.alloc(0); // 默认空 Buffer
        if (iv !== undefined) {
            if (typeof iv === 'string') {
                ivBuffer = Buffer.from(iv, encoding);
            }
            else if (Buffer.isBuffer(iv)) {
                ivBuffer = iv;
            }
            else {
                throw new TypeError('aesEncrypt: 初始化向量必须是字符串、Buffer 或 undefined');
            }
        }
        // 原有校验逻辑（更新为适配后的 Buffer 变量）
        if (!mode.startsWith('aes-')) {
            throw new Error('aesEncrypt: 模式必须是 AES 类型（如 aes-128-cbc）');
        }
        // 额外校验：密钥长度必须匹配加密模式（可选，增强健壮性）
        var keyLength = keyBuffer.length * 8; // 字节 → 位
        if (!mode.includes("".concat(keyLength, "-"))) {
            console.warn("aesEncrypt: \u5BC6\u94A5\u957F\u5EA6\uFF08".concat(keyLength, "\u4F4D\uFF09\u53EF\u80FD\u4E0E\u6A21\u5F0F\uFF08").concat(mode, "\uFF09\u4E0D\u5339\u914D\uFF0C\u53EF\u80FD\u5BFC\u81F4\u52A0\u5BC6\u5931\u8D25"));
        }
        // 创建加密实例（使用适配后的 Buffer）
        var cipher = crypto_1.default.createCipheriv(mode, keyBuffer, ivBuffer);
        // 执行加密（拼接加密结果）
        var encrypted = Buffer.concat([cipher.update(dataBuffer), cipher.final()]);
        return encrypted;
    },
    /**
   * MD5 加密（返回 32 位小写哈希字符串）
   * @param str 待加密的字符串
   * @returns MD5 哈希字符串（32位小写）
   */
    md5: function (str) {
        if (typeof str !== 'string')
            throw new TypeError('md5: 输入必须是字符串');
        return crypto_1.default.createHash('md5').update(str).digest('hex');
    },
    /**
     * 生成随机字符串（基于加密安全的随机数生成器）
     * @param size 随机字符串长度（默认 16 字节）
     * @returns 随机字符串（hex 编码，长度为 size*2）
     */
    randomBytes: function (size) {
        if (size === void 0) { size = 16; }
        if (typeof size !== 'number' || size < 1)
            throw new Error('randomBytes: 长度必须是正整数');
        return crypto_1.default.randomBytes(size).toString('hex');
    },
    /**
     * RSA 加密（公钥加密，通常用于加密短数据如 AES 密钥）
     * @param buffer 待加密的 Buffer 数据（长度不能超过 RSA 密钥长度-11 字节）
     * @param key RSA 公钥（Buffer 或字符串格式，支持 PEM 格式）
     * @returns 加密后的 Buffer（默认 base64 编码可自行转换）
     */
    rsaEncrypt: function (buffer, key) {
        if (!Buffer.isBuffer(buffer))
            throw new TypeError('rsaEncrypt: 数据必须是 Buffer 类型');
        if (typeof key !== 'string' && !Buffer.isBuffer(key))
            throw new TypeError('rsaEncrypt: 公钥必须是字符串或 Buffer 类型');
        // RSA 加密（OAEP 填充模式，更安全）
        var encrypted = crypto_1.default.publicEncrypt({
            key: key,
            padding: crypto_1.default.constants.RSA_PKCS1_OAEP_PADDING, // 推荐 OAEP 填充（比 PKCS1 更安全）
        }, buffer);
        return encrypted;
    },
};
/**
 * 压缩工具类：zlib 解压/压缩
 */
var zlibTools = {
    /**
     * 解压数据（对应 zlib.inflate）
     * @param buffer 待解压的 Buffer 数据
     * @returns 解压后的 Buffer
     */
    inflate: function (buffer) { return __awaiter(void 0, void 0, void 0, function () {
        var err_1;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    if (!Buffer.isBuffer(buffer))
                        throw new TypeError('inflate: 数据必须是 Buffer 类型');
                    _a.label = 1;
                case 1:
                    _a.trys.push([1, 3, , 4]);
                    return [4 /*yield*/, inflateAsync(buffer)];
                case 2: return [2 /*return*/, _a.sent()];
                case 3:
                    err_1 = _a.sent();
                    console.error('zlib 解压失败:', err_1);
                    throw err_1; // 抛出错误供上层捕获
                case 4: return [2 /*return*/];
            }
        });
    }); },
    /**
     * 压缩数据（对应 zlib.deflate）
     * @param buffer 待压缩的 Buffer 数据
     * @returns 压缩后的 Buffer
     */
    deflate: function (buffer) { return __awaiter(void 0, void 0, void 0, function () {
        var err_2;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    if (!Buffer.isBuffer(buffer))
                        throw new TypeError('deflate: 数据必须是 Buffer 类型');
                    _a.label = 1;
                case 1:
                    _a.trys.push([1, 3, , 4]);
                    return [4 /*yield*/, deflateAsync(buffer)];
                case 2: return [2 /*return*/, _a.sent()];
                case 3:
                    err_2 = _a.sent();
                    console.error('zlib 压缩失败:', err_2);
                    throw err_2; // 抛出错误供上层捕获
                case 4: return [2 /*return*/];
            }
        });
    }); },
};
/**
 * 挂载到 globalThis.lx.utils（全局可访问）
 */
if (!globalThis.lx) {
    globalThis.lx = {}; // 初始化 lx 命名空间
}
if (!globalThis.lx.utils) {
    globalThis.lx.utils = {};
}
// // 赋值工具类到全局命名空间
// globalThis.lx.utils.buffer = buffer;
// globalThis.lx.utils.crypto = cryptoTools as any;
// globalThis.lx.utils.zlib = zlibTools as any;
/**
 * 导出工具类（可选：支持模块化导入，也可直接通过 globalThis 访问）
 */
exports.lxUtils = {
    buffer: buffer,
    crypto: cryptoTools,
    zlib: zlibTools,
};
exports.default = exports.lxUtils;
