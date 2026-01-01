/**
 * 实现 globalThis.lx.utils 工具接口
 * 兼容 Node.js 核心 API：Buffer、加密、压缩
 */
import crypto from 'crypto';
import zlib from 'zlib';
import { promisify } from 'util';

//  promisify zlib 异步方法（适配 Promise 语法）
const inflateAsync = promisify(zlib.inflate);
const deflateAsync = promisify(zlib.deflate);

/**
 * Buffer 工具类：对应 Node.js Buffer
 */
const buffer = {
  /**
   * 从数据创建 Buffer（兼容 Node.js Buffer.from）
   * @param data 输入数据（字符串、数组、ArrayBuffer 等）
   * @param encoding 编码格式（可选，如 'utf8'、'base64'）
   * @returns Buffer 实例
   */
  from: (data: Buffer | string | ArrayBuffer | Uint8Array, encoding?: BufferEncoding): Buffer => {
    if (typeof data === 'string') {
      return Buffer.from(data, encoding);
    }
    if (data instanceof ArrayBuffer) {
      return Buffer.from(new Uint8Array(data));
    }
    return Buffer.from(data as Uint8Array | Buffer);
  },

  /**
   * Buffer 转字符串（对应 Node.js Buffer.toString）
   * @param buffer 要转换的 Buffer
   * @param format 编码格式（如 'utf8'、'base64'、'hex'，默认 'utf8'）
   * @returns 转换后的字符串
   */
  bufToString: (buffer: Buffer, format: BufferEncoding = 'utf8'): string => {
    if (!Buffer.isBuffer(buffer)) throw new TypeError('bufToString 参数必须是 Buffer 类型');
    return buffer.toString(format);
  },
};

/**
 * 加密工具类：AES、MD5、RSA、随机字符串
 */
const cryptoTools = {
    /**
     * AES 加密（支持字符串/Buffer 类型输入：数据、密钥、IV）
     * @param buffer 待加密的数据（字符串或 Buffer，默认 utf8 编码）
     * @param mode 加密模式（如 'aes-128-cbc'、'aes-256-gcm'，需包含密钥长度和模式）
     * @param key 加密密钥（字符串或 Buffer，长度需匹配模式：128位=16字节，256位=32字节，默认 utf8 编码）
     * @param iv 初始化向量（字符串、Buffer 或 undefined，CBC 模式必需，默认 utf8 编码）
     * @param encoding 字符串编码（可选，默认 'utf8'，支持 'gbk'、'base64' 等）
     * @returns 加密后的 Buffer
     */
    aesEncrypt: (
        buffer: Buffer | string,
        mode: string,
        key: Buffer | string,
        iv?: Buffer | string | undefined,
        encoding: BufferEncoding = 'utf8' // 统一字符串编码参数（数据、key、iv 共用）
        ): Buffer => {
        // 1. 数据（buffer）适配：字符串 → Buffer
        let dataBuffer: Buffer;
        if (typeof buffer === 'string') {
            dataBuffer = Buffer.from(buffer, encoding);
        } else if (Buffer.isBuffer(buffer)) {
            dataBuffer = buffer;
        } else {
            throw new TypeError('aesEncrypt: 数据必须是字符串或 Buffer 类型');
        }

        // 2. 密钥（key）适配：字符串 → Buffer
        let keyBuffer: Buffer;
        if (typeof key === 'string') {
            keyBuffer = Buffer.from(key, encoding);
        } else if (Buffer.isBuffer(key)) {
            keyBuffer = key;
        } else {
            throw new TypeError('aesEncrypt: 密钥必须是字符串或 Buffer 类型');
        }

        // 3. 初始化向量（iv）适配：字符串 → Buffer（支持 undefined）
        let ivBuffer: Buffer = Buffer.alloc(0); // 默认空 Buffer
        if (iv !== undefined) {
            if (typeof iv === 'string') {
            ivBuffer = Buffer.from(iv, encoding);
            } else if (Buffer.isBuffer(iv)) {
            ivBuffer = iv;
            } else {
            throw new TypeError('aesEncrypt: 初始化向量必须是字符串、Buffer 或 undefined');
            }
        }

        // 原有校验逻辑（更新为适配后的 Buffer 变量）
        if (!mode.startsWith('aes-')) {
            throw new Error('aesEncrypt: 模式必须是 AES 类型（如 aes-128-cbc）');
        }

        // 额外校验：密钥长度必须匹配加密模式（可选，增强健壮性）
        const keyLength = keyBuffer.length * 8; // 字节 → 位
        if (!mode.includes(`${keyLength}-`)) {
            console.warn(`aesEncrypt: 密钥长度（${keyLength}位）可能与模式（${mode}）不匹配，可能导致加密失败`);
        }

        // 创建加密实例（使用适配后的 Buffer）
        const cipher = crypto.createCipheriv(mode, keyBuffer, ivBuffer);
        // 执行加密（拼接加密结果）
        const encrypted = Buffer.concat([cipher.update(dataBuffer), cipher.final()]);
        return encrypted;
    },


    /**
   * MD5 加密（返回 32 位小写哈希字符串）
   * @param str 待加密的字符串
   * @returns MD5 哈希字符串（32位小写）
   */
    md5: (str: string): string => {
        if (typeof str !== 'string') throw new TypeError('md5: 输入必须是字符串');
        return crypto.createHash('md5').update(str).digest('hex');
    },

  /**
   * 生成随机字符串（基于加密安全的随机数生成器）
   * @param size 随机字符串长度（默认 16 字节）
   * @returns 随机字符串（hex 编码，长度为 size*2）
   */
  randomBytes: (size: number = 16): string => {
    if (typeof size !== 'number' || size < 1) throw new Error('randomBytes: 长度必须是正整数');
    return crypto.randomBytes(size).toString('hex');
  },

  /**
   * RSA 加密（公钥加密，通常用于加密短数据如 AES 密钥）
   * @param buffer 待加密的 Buffer 数据（长度不能超过 RSA 密钥长度-11 字节）
   * @param key RSA 公钥（Buffer 或字符串格式，支持 PEM 格式）
   * @returns 加密后的 Buffer（默认 base64 编码可自行转换）
   */
  rsaEncrypt: (buffer: Buffer, key: Buffer | string): Buffer => {
    if (!Buffer.isBuffer(buffer)) throw new TypeError('rsaEncrypt: 数据必须是 Buffer 类型');
    if (typeof key !== 'string' && !Buffer.isBuffer(key)) throw new TypeError('rsaEncrypt: 公钥必须是字符串或 Buffer 类型');

    // RSA 加密（OAEP 填充模式，更安全）
    const encrypted = crypto.publicEncrypt(
      {
        key,
        padding: crypto.constants.RSA_PKCS1_OAEP_PADDING, // 推荐 OAEP 填充（比 PKCS1 更安全）
      },
      buffer
    );
    return encrypted;
  },
};

/**
 * 压缩工具类：zlib 解压/压缩
 */
const zlibTools = {
  /**
   * 解压数据（对应 zlib.inflate）
   * @param buffer 待解压的 Buffer 数据
   * @returns 解压后的 Buffer
   */
  inflate: async (buffer: Buffer): Promise<Buffer> => {
    if (!Buffer.isBuffer(buffer)) throw new TypeError('inflate: 数据必须是 Buffer 类型');
    try {
      return await inflateAsync(buffer);
    } catch (err) {
      console.error('zlib 解压失败:', err);
      throw err; // 抛出错误供上层捕获
    }
  },

  /**
   * 压缩数据（对应 zlib.deflate）
   * @param buffer 待压缩的 Buffer 数据
   * @returns 压缩后的 Buffer
   */
  deflate: async (buffer: Buffer): Promise<Buffer> => {
    if (!Buffer.isBuffer(buffer)) throw new TypeError('deflate: 数据必须是 Buffer 类型');
    try {
      return await deflateAsync(buffer);
    } catch (err) {
      console.error('zlib 压缩失败:', err);
      throw err; // 抛出错误供上层捕获
    }
  },
};

/**
 * 挂载到 globalThis.lx.utils（全局可访问）
 */
if (!globalThis.lx) {
  globalThis.lx = {} as any; // 初始化 lx 命名空间
}
if (!globalThis.lx.utils) {
  globalThis.lx.utils = {} as any;
}

// // 赋值工具类到全局命名空间
// globalThis.lx.utils.buffer = buffer;
// globalThis.lx.utils.crypto = cryptoTools as any;
// globalThis.lx.utils.zlib = zlibTools as any;

/**
 * 导出工具类（可选：支持模块化导入，也可直接通过 globalThis 访问）
 */
export const lxUtils = {
  buffer,
  crypto: cryptoTools,
  zlib: zlibTools,
};

export default lxUtils;