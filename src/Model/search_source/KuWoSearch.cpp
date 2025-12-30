#include "KuWoSearch.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslSocket>
#include <QUrlQuery>

KuWoSearch::KuWoSearch(QObject *parent)
    : ISearchSource(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &KuWoSearch::onReplyFinished);
}

void KuWoSearch::searchMusic(const QString songName)
{
    searchMusic(songName, 1, 200);  // 默认第1页，200条
}

void KuWoSearch::searchMusic(const QString &keyword, int page, int limit)
{
    const QString baseUrl = "http://search.kuwo.cn/r.s";

    // 2. 构建 URL 参数（用 QUrlQuery 管理，自动处理拼接和编码）
    QUrlQuery query;
    // 固定参数（抽离出来，便于修改）
    query.addQueryItem("client", "kt");
    query.addQueryItem("uid", "794762570");
    query.addQueryItem("ver", "kwplayer_ar_9.2.2.1");
    query.addQueryItem("vipver", "1");
    query.addQueryItem("show_copyright_off", "1");
    query.addQueryItem("newver", "1");
    query.addQueryItem("ft", "music");
    query.addQueryItem("cluster", "0");
    query.addQueryItem("strategy", "2012");
    query.addQueryItem("encoding", "utf8");
    query.addQueryItem("rformat", "json");
    query.addQueryItem("vermerge", "1");
    query.addQueryItem("mobi", "1");
    query.addQueryItem("issubtitle", "1");
    
    // 动态参数（插入变量，自动编码）
    query.addQueryItem("all", encodeURIComponent(keyword)); // 关键词编码
    query.addQueryItem("pn", QString::number(page - 1));     // 页码（page-1）
    query.addQueryItem("rn", QString::number(limit));        // 每页数量

    // 3. 组合 URL 和参数（Qt 自动处理拼接）
    QUrl url(baseUrl);
    url.setQuery(query); // 自动拼接参数，无需手动加 ?&

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    request.setRawHeader("Referer", "https://www.kuwo.cn/");

    // 酷我API通常需要csrf和cookie，这里仅做演示，实际项目需处理cookie和csrf
    request.setRawHeader("csrf", "");// 可根据实际情况设置

    // QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    // sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    // sslConf.setProtocol(QSsl::TlsV1_2OrLater);
    // request.setSslConfiguration(sslConf);

    QNetworkReply *reply = m_manager->get(request);
    // reply->ignoreSslErrors();
}

void KuWoSearch::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit searchError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        emit searchError("JSON解析失败");
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray abslist = root["abslist"].toArray();

    if (abslist.isEmpty()) {
        emit searchError("API返回错误");
        reply->deleteLater();
        return;
    }

    try {
        QList<SongInfo> songs = handleSearchResult(abslist);
        emit searchFinished(songs);
    } catch (const std::exception &e) {
        emit searchError(QString("处理结果出错: %1").arg(e.what()));
    }

    reply->deleteLater();
}

QList<SongInfo> KuWoSearch::handleSearchResult(const QJsonArray &abslist)
{
    QList<SongInfo> result;

    for (const QJsonValue &value : abslist) {
        QJsonObject item = value.toObject();

        SongInfo info;
        info.name = item["NAME"].toString();
        info.singer = item["ARTIST"].toString("未知艺术家");
        info.album = item["ALBUM"].toString("未知专辑");
        info.duration = item["DURATION"].toString().toInt();
        info.interval = formatTime(info.duration); 
        info.source = "kw";
        info.songmid = item["MUSICRID"].toString().split("_").last(); // MUSICRID格式通常为 "MUSIC_12345678"

        result.append(info);
    }

    return result;
}

QString KuWoSearch::formatTime(int seconds)
{
    int mins = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
}