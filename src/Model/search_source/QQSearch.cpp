#include "QQSearch.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslSocket>
#include <QUrlQuery>

QQSearch::QQSearch(QObject *parent)
    : ISearchSource(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &QQSearch::onReplyFinished);
}

void QQSearch::searchMusic(const QString songName)
{
    searchMusic(songName, 1, 50);  // 默认第1页，100条
}

void QQSearch::searchMusic(const QString keyword, int page, int limit)
{
    // 1. 构建请求 URL
    QUrl url("https://u.y.qq.com/cgi-bin/musicu.fcg");

    // 2. 构建请求对象，设置请求头
    QNetworkRequest request(url);
    // 设置 POST 核心请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); // JSON 格式请求体
    request.setHeader(QNetworkRequest::UserAgentHeader, "QQMusic 14090508(android 12)"); // 模拟客户端 UA
    request.setRawHeader("Connection", "keep-alive"); // 复用连接
    request.setRawHeader("Accept", "application/json"); // 接收 JSON 响应

    // 3. 构建 JSON 请求体（对应 JS 的 body 部分）
    QJsonObject commObj; // comm 子对象
    commObj.insert("ct", 11);
    commObj.insert("cv", 14090508);
    commObj.insert("v", 14090508);
    commObj.insert("tmeAppID", "qqmusic");
    commObj.insert("phonetype", "EBG-AN10");
    commObj.insert("deviceScore", "553.47");
    commObj.insert("devicelevel", 50);
    commObj.insert("newdevicelevel", 20);
    commObj.insert("rom", "HuaWei/EMOTION/EmotionUI_14.2.0");
    commObj.insert("os_ver", "12");
    commObj.insert("OpenUDID", 0);
    commObj.insert("OpenUDID2", 0);
    commObj.insert("QIMEI36", 0);
    commObj.insert("udid", 0);
    commObj.insert("chid", 0);
    commObj.insert("aid", 0);
    commObj.insert("oaid", 0);
    commObj.insert("taid", 0);
    commObj.insert("tid", 0);
    commObj.insert("wid", 0);
    commObj.insert("uid", 0);
    commObj.insert("sid", 0);
    commObj.insert("modeSwitch", 6);
    commObj.insert("teenMode", 0);
    commObj.insert("ui_mode", 2);
    commObj.insert("nettype", 1020);
    commObj.insert("v4ip", "");

    QJsonObject paramObj; // req.param 子对象
    paramObj.insert("search_type", 0);
    paramObj.insert("query", keyword); // 搜索关键词
    paramObj.insert("page_num", page); // 页码
    paramObj.insert("num_per_page", limit); // 每页数量
    paramObj.insert("highlight", 0);
    paramObj.insert("nqc_flag", 0);
    paramObj.insert("multi_zhida", 0);
    paramObj.insert("cat", 2);
    paramObj.insert("grp", 1);
    paramObj.insert("sin", 0);
    paramObj.insert("sem", 0);

    QJsonObject reqObj; // req 子对象
    reqObj.insert("module", "music.search.SearchCgiService");
    reqObj.insert("method", "DoSearchForQQMusicMobile");
    reqObj.insert("param", paramObj);

    QJsonObject bodyObj; // 根请求体
    bodyObj.insert("comm", commObj);
    bodyObj.insert("req", reqObj);

    // 4. 将 JSON 对象转为二进制数据（POST 请求体）
    QJsonDocument jsonDoc(bodyObj);
    QByteArray postData = jsonDoc.toJson(QJsonDocument::Compact); // Compact 去掉冗余空格，减小体积

    QNetworkReply *reply = m_manager->post(request, postData);
}

void QQSearch::onReplyFinished(QNetworkReply *reply)
{
    if (!reply) return;
    if (reply->error() != QNetworkReply::NoError) {
        emit searchError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        emit searchError("JSON 解析失败");
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();            // 从文件中获得对象
    int code = root["code"].toInt();        // 检查状态码
    int reqCode = -1;
    // 1. 先检查 rootObj 包含 "req"，且 "req" 是对象
    if (root.contains("req") && root["req"].isObject()) {
        QJsonObject reqObj = root["req"].toObject();
        // 2. 从 reqObj 中取 code
        if (reqObj.contains("code")) {
            reqCode = reqObj["code"].toInt();
        }
    }
    if (code != 0 || reqCode != 0) {
        emit searchError("API返回错误");
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

QList<SongInfo> QQSearch::handleSearchResult(const QJsonDocument &doc)
{
    QList<SongInfo> result;
    QJsonObject root = doc.object();
    QJsonArray data = root.value("req").toObject()
                        .value("data").toObject()
                        .value("body").toObject()
                        .value("item_song").toArray();
    

    for (const QJsonValue &value : data) {
        QJsonObject item = value.toObject();

        SongInfo info;
        info.name = item["name"].toString();
        info.singer = item.value("singer").toArray()
                        [0].toObject()
                        .value("name")
                        .toString("未知艺术家");
        info.album = item.value("album").toObject()
                        .value("name")
                        .toString("未知专辑");
        info.duration = item["interval"].toInt();
        info.interval = formatTime(info.duration); 
        info.source = "QQ";
        info.songmid = item["mid"].toString();

        result.append(info);
    }
    return result;
}

QString QQSearch::formatTime(int seconds)
{
    int mins = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
}