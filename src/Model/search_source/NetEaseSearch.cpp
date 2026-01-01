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