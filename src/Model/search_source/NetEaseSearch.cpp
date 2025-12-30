#include "NetEaseSearch.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslSocket>
#include <QUrlQuery>

NetEaseSearch::NetEaseSearch(QObject *parent)
    : ISearchSource(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &NetEaseSearch::onReplyFinished);
}

void NetEaseSearch::searchMusic(const QString songName)
{
    searchMusic(songName, 1, 50);  // 默认第1页，50条
}

// void NetEaseSearch::searchMusic(const QString &keyword, int page, int limit)
// {
//     QString url = "/api/cloudsearch/pc";
//     // 2. 构建搜索参数（和 JS 一致）
//     QVariantMap searchData;
//     searchData.insert("s", keyword);          // 搜索关键词
//     searchData.insert("type", 1);         // 1=单曲
//     searchData.insert("limit", limit);    // 每页数量
//     searchData.insert("total", (page == 1)); // page=1 为 true
//     searchData.insert("offset", limit * (page - 1)); // 偏移量
    
//     // 1. 请求 URL
//     QUrl requestUrl("http://interface.music.163.com/eapi/batch");

//     // 2. 设置请求头（和 JS 完全一致）
//     QNetworkRequest request(requestUrl);
//     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
//     request.setHeader(QNetworkRequest::UserAgentHeader, 
//                       "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36");
//     request.setRawHeader("origin", "https://music.163.com");
//     // Cookie（可替换为自己的有效 Cookie）
//     QString cookie = "os=pc; deviceId=A9C064BB4584D038B1565B58CB05F95290998EE8B025AA2D07AE; osver=Microsoft-Windows-10-Home-China-build-19043-64bit; appver=2.5.2.197409; channel=netease; MUSIC_A=37a11f2eb9de9930cad479b2ad495b0e4c982367fb6f909d9a3f18f876c6b49faddb3081250c4980dd7e19d4bd9bf384e004602712cf2b2b8efaafaab164268a00b47359f85f22705cc95cb6180f3aee40f5be1ebf3148d888aa2d90636647d0c3061cd18d77b7a0; __csrf=05b50d54082694f945d7de75c210ef94; mode=Z7M-KP5(7)GZ; NMTID=00OZLp2VVgq9QdwokUgq3XNfOddQyIAAAF_6i8eJg; ntes_kaola_ad=1";
//     request.setRawHeader("Cookie", cookie.toUtf8());

//     // 3. 构建加密表单请求体
//     QUrlQuery formData = eapi(url, searchData);
//     QByteArray postData = formData.toString(QUrl::FullyEncoded).toUtf8();

//     // 4. 发送 POST 请求
//     QNetworkReply *reply = m_manager->post(request, postData);
//     reply->ignoreSslErrors();
// }
void NetEaseSearch::searchMusic(const QString &keyword, int page, int limit){
    // 1. 计算偏移量（和 JS 一致）
    int offset = (page - 1) * limit;

    // 2. 构建 URL 参数（编码关键词，拼接路径）
    QUrl url("https://music.163.com/api/cloudsearch/pc");
    QUrlQuery query;
    query.addQueryItem("s", QUrl::toPercentEncoding(keyword)); // 对应 encodeURIComponent
    query.addQueryItem("type", "1"); // 单曲
    query.addQueryItem("offset", QString::number(offset));
    query.addQueryItem("limit", QString::number(limit));
    url.setQuery(query);

    // 3. 构建请求，设置请求头（和 JS 完全一致）
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    request.setRawHeader("Referer", "https://music.163.com/");
    request.setRawHeader("Origin", "https://music.163.com");
    // 可选：关闭 HTTPS 证书校验（某些环境需要，对应 JS 的 rejectUnauthorized: false）
    // QSslConfiguration config = request.sslConfiguration();
    // config.setPeerVerifyMode(QSslSocket::VerifyNone);
    // request.setSslConfiguration(config);

    // 4. 发送 GET 请求（核心：GET 方法，无加密/表单）
    QNetworkReply* reply = m_manager->get(request);
}

void printReplyInfo(QNetworkReply* reply);
void NetEaseSearch::onReplyFinished(QNetworkReply *reply)
{
    if (!reply) return;
    if (reply->error() != QNetworkReply::NoError) {
        emit searchError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    // QString decrypted = aesDecryptECB(QByteArray::fromHex(data), eapiKey);
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        emit searchError("JSON 解析失败");
        reply->deleteLater();
        return;
    }

    int code = doc.object().value("code").toInt(-1);
    if (code != 200) {
        emit searchError(QString("搜索失败，错误码：%1").arg(code));
        reply->deleteLater();
        return;
    }

    try {
        QList<SongInfo> songs = handleSearchResult(doc);
        emit searchFinished(songs);
    } catch (...) {
        emit searchError("处理结果出错");
    }

    reply->deleteLater();
}

QList<SongInfo> NetEaseSearch::handleSearchResult(const QJsonDocument &doc)
{
    QList<SongInfo> result;
    QJsonObject root = doc.object();
    QJsonObject res = root.value("result").toObject();
    QJsonArray songs = res.value("songs").toArray();

    for (const QJsonValue &value : songs) {
        QJsonObject item = value.toObject();

        SongInfo info;
        info.name = item["name"].toString();
        info.singer = item.value("ar").toArray()
                        [0].toObject()
                        .value("name").toString("未知艺术家");
        info.album = item.value("al").toObject()
                        .value("name").toString("未知专辑");
        info.duration = item["dt"].toInt()/1000;
        info.interval = formatTime(info.duration); 
        info.source = "NetEase";
        info.songmid = item["id"].toString();

        result.append(info);
    }

    return result;
}

QString NetEaseSearch::formatTime(int seconds)
{
    int mins = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
}

// -------------------------- 2. AES-128-ECB 加密实现（对应 JS aesEncrypt） --------------------------
/**
 * @brief AES-128-ECB 加密，返回十六进制大写字符串
 * @param data 待加密的原始数据
 * @param key 加密密钥（eapiKey）
 * @return 加密后的十六进制大写字符串
 */
QString NetEaseSearch::aesEncryptECB(const QByteArray& data, const QString& key) {
    QAESEncryption aes(QAESEncryption::AES_128, QAESEncryption::ECB);
    // 加密：AES-128-ECB + PKCS7 填充
    QByteArray encrypted = aes.encode(data, key.toUtf8());
    // 转为十六进制大写字符串（对应 JS toString('hex').toUpperCase()）
    return QString(encrypted.toHex()).toUpper();
}

// -------------------------- 3. EAPI 加密核心函数（对应 JS eapi 函数） --------------------------
/**
 * @brief 实现网易云 EAPI 加密逻辑
 * @param url 请求路径（如 "/api/cloudsearch/pc"）
 * @param data 搜索参数（QVariantMap 转 JSON 字符串）
 * @return 加密后的表单参数（params=加密字符串）
 */
QUrlQuery NetEaseSearch::eapi(const QString& url, const QVariantMap& data) {
    // 1. 将 data 转为 JSON 字符串（对应 JS JSON.stringify(object)）
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(data);
    QString text = jsonDoc.toJson(QJsonDocument::Compact);
    
    // 2. 构建 MD5 消息体：`nobody${url}use${text}md5forencrypt`
    QString message = QString("nobody%1use%2md5forencrypt").arg(url).arg(text);
    
    // 3. MD5 哈希（对应 JS createHash('md5').update(message).digest('hex')）
    QByteArray md5Digest = QCryptographicHash::hash(message.toUtf8(), QCryptographicHash::Md5);
    QString digest = QString(md5Digest.toHex()); // 转为十六进制字符串
    
    // 4. 构建加密原始数据：${url}-36cd479b6b5-${text}-36cd479b6b5-${digest}
    QString rawData = QString("%1-36cd479b6b5-%2-36cd479b6b5-%3").arg(url).arg(text).arg(digest);
    
    // 5. AES-128-ECB 加密（对应 JS aesEncrypt）
    QString encryptedParams = aesEncryptECB(rawData.toUtf8(), eapiKey);
    
    // 6. 构建表单参数（params=加密字符串）
    QUrlQuery formData;
    formData.addQueryItem("params", encryptedParams);
    return formData;
}

void printReplyInfo(QNetworkReply* reply) {
    qDebug() << "========================================";
    qDebug() << "【1. 请求基本信息】";
    qDebug() << "请求 URL：" << reply->request().url().toString();
    // qDebug() << "请求方法：" << reply->request().attribute(QNetworkRequest::HttpMethodAttribute).toString();

    qDebug() << "\n【2. 响应状态信息】";
    // HTTP 状态码（如 200/403/500）
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // qDebug() << "HTTP 状态码：" << statusCode << "(" << QNetworkReply::attributeReason(statusCode) << ")";
    qDebug() << "HTTP 状态码：" << statusCode;
    // 响应头（关键：Content-Length/Content-Type 等）
    QList<QByteArray> headerKeys = reply->rawHeaderList();
    qDebug() << "响应头数量：" << headerKeys.size();
    for (const QByteArray& key : headerKeys) {
        qDebug() << "  " << key << "：" << reply->rawHeader(key);
    }

    qDebug() << "\n【3. 响应内容信息】";
    // 读取响应内容（原始字节）
    QByteArray body = reply->readAll();
    qDebug() << "响应内容长度（字节）：" << body.size();
    if (body.isEmpty()) {
        qDebug() << "响应内容：空（""）";
    } else {
        // 尝试以字符串打印（UTF-8）
        qDebug() << "响应内容（UTF-8）：\n" << QString::fromUtf8(body);
        // 可选：打印十六进制（排查二进制/加密内容）
        qDebug() << "响应内容（十六进制）：\n" << body.toHex(' ');
    }

    qDebug() << "\n【4. 错误信息（如有）】";
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "错误码：" << reply->error();
        qDebug() << "错误描述：" << reply->errorString();
        // 扩展错误信息（Qt 5.15+）
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        // qDebug() << "扩展错误信息：" << reply->errorString(QNetworkReply::NativeError);
        #endif
    } else {
        qDebug() << "无错误";
    }
    qDebug() << "========================================\n";
}