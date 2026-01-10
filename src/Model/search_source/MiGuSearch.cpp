#include "MiGuSearch.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslSocket>
#include <QUrlQuery>

MiGuSearch::MiGuSearch(QObject *parent)
    : ISearchSource(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &MiGuSearch::onReplyFinished);
}

void MiGuSearch::searchMusic(const QString songName)
{
    searchMusic(songName, 1, 50);
}

void MiGuSearch::searchMusic(const QString keyword, int page, int limit)
{
    // 1. 生成毫秒级时间戳（对应 JS 的 Date.now().toString()）
    QString time = QString::number(QDateTime::currentMSecsSinceEpoch());

    // 2. 生成签名数据（调用完整的签名函数）
    SignData signData = createSignature(time, keyword);

    // 3. 构建请求URL（拼接所有参数）
    QUrl url("https://jadeite.migu.cn/music_search/v3/search/searchAll");
    QUrlQuery query;
    // 固定参数
    query.addQueryItem("isCorrect", "0");
    query.addQueryItem("isCopyright", "1");
    // searchSwitch：原始JSON字符串，Qt自动URL编码（和JS转义效果一致）
    query.addQueryItem("searchSwitch", "{\"song\":1,\"album\":0,\"singer\":0,\"tagSong\":1,\"mvSong\":0,\"bestShow\":1,\"songlist\":0,\"lyricSong\":0}");
    // 动态参数
    query.addQueryItem("pageSize", QString::number(limit));
    query.addQueryItem("text", QUrl::toPercentEncoding(keyword)); // 编码关键词
    query.addQueryItem("pageNo", QString::number(page));
    query.addQueryItem("sort", "0");
    query.addQueryItem("sid", "USS");
    url.setQuery(query);

    // 4. 构建请求头（和JS完全一致）
    QNetworkRequest request(url);
    // 自定义请求头
    request.setRawHeader("uiVersion", "A_music_3.6.1");
    request.setRawHeader("deviceId", signData.deviceId.toUtf8());
    request.setRawHeader("timestamp", time.toUtf8());
    request.setRawHeader("sign", signData.sign.toUtf8());
    request.setRawHeader("channel", "0146921");
    // User-Agent（和JS完全一致）
    request.setHeader(QNetworkRequest::UserAgentHeader, 
                      "Mozilla/5.0 (Linux; U; Android 11.0.0; zh-cn; MI 11 Build/OPR1.170623.032) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30");
    request.setTransferTimeout(5000); // 设置超时时间为 5 秒
    // 5. 发送GET请求
    
    QNetworkReply* reply = m_manager->get(request);
}

void MiGuSearch::onReplyFinished(QNetworkReply *reply)
{
    if (!reply) return;
    if (reply->error() != QNetworkReply::NoError) {
        emit searchError(reply->errorString());
        reply->deleteLater();
        return;
    }

    const QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        emit searchError("JSON 解析失败");
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

QList<SongInfo> MiGuSearch::handleSearchResult(const QJsonDocument &doc)
{
    QList<SongInfo> result;
    QJsonObject root = doc.object();
    QJsonObject res = root.value("songResultData").toObject();
    QJsonArray songs = res.value("resultList").toArray();

    for (const QJsonValue &value : songs) {
        QJsonArray array = value.toArray();
        QJsonObject item = array[0].toObject();

        SongInfo info;
        info.name = item["name"].toString();
        info.singer = item.value("singerList").toArray()
                        [0].toObject()
                        .value("name").toString("未知艺术家");
        info.album = item.value("album").toString("未知专辑");
        info.duration = item["duration"].toInt();
        info.interval = formatTime(info.duration); 
        info.source = "MiGu";
        info.songmid = item["id"].toString();

        result.append(info);
    }

    return result;
}

QString MiGuSearch::formatTime(int seconds)
{
    int mins = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
}

// -------------------------- 3. MD5 加密函数（对应 JS 的 toMD5） --------------------------
/**
 * @brief 生成MD5哈希值（小写十六进制）
 * @param data 待加密的字符串
 * @return MD5加密后的小写字符串
 */
QString MiGuSearch::toMD5(const QString& data) {
    QByteArray byteData = data.toUtf8();
    QByteArray md5Bytes = QCryptographicHash::hash(byteData, QCryptographicHash::Md5);
    return QString(md5Bytes.toHex()).toLower(); // JS的MD5通常返回小写，需统一
}

// -------------------------- 4. 签名生成函数（完整实现，对应 JS 的 createSignature） --------------------------
MiGuSearch::SignData MiGuSearch::createSignature(const QString& time, const QString& str) {
    SignData signData;
    // 1. 设备ID固定（和JS一致）
    signData.deviceId = FIXED_DEVICE_ID;
    
    // 2. 拼接签名原始字符串：${str}${signatureMd5}yyapp2d16148780a1dcc7408e06336b98cfd50${deviceId}${time}
    QString rawSign = QString("%1%2%3%4%5")
                          .arg(str)          // 搜索关键词
                          .arg(SIGNATURE_MD5) // 固定盐值1
                          .arg(FIXED_SALT)    // 固定盐值2
                          .arg(FIXED_DEVICE_ID) // 设备ID
                          .arg(time);        // 时间戳
    
    // 3. MD5加密生成签名
    signData.sign = toMD5(rawSign);
    return signData;
}