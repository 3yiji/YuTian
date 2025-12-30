#include "KuGouSearch.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslSocket>
#include <QUrlQuery>

KuGouSearch::KuGouSearch(QObject *parent)
    : ISearchSource(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &KuGouSearch::onReplyFinished);
}

// 实现接口方法（调用带分页的版本）
void KuGouSearch::searchMusic(const QString songName)
{
    searchMusic(songName, 1, 100);  // 默认第1页，100条
}

void KuGouSearch::searchMusic(const QString &keyword, int page, int limit)
{
    // 按 TS 版参数构造请求
    QUrl url("https://ioscdn.kugou.com/api/v3/search/song");
    QUrlQuery query;
    query.addQueryItem("keyword", keyword);
    query.addQueryItem("page", QString::number(page));
    query.addQueryItem("pagesize", QString::number(limit));
    query.addQueryItem("showtype", "10");
    query.addQueryItem("plat", "2");
    query.addQueryItem("version", "7910");
    query.addQueryItem("tag", "1");
    query.addQueryItem("correct", "1");
    query.addQueryItem("privilege", "1");
    query.addQueryItem("sver", "5");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    request.setRawHeader("Referer", "https://kugou.com");

    // 调试环境关闭证书校验以绕过域名不匹配；生产务必移除
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::TlsV1_2OrLater);       // 与目标服务兼容
    request.setSslConfiguration(sslConf);

    QNetworkReply *reply = m_manager->get(request);

    // 调试环境：立即忽略所有 SSL 错误（生产务必移除）
    reply->ignoreSslErrors();
}

void KuGouSearch::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {         // 发生错误直接返回
        emit searchError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();             // 读取响应数据
    QJsonDocument doc = QJsonDocument::fromJson(data);  // 解析JSON，得到json文件

    if (doc.isNull() || !doc.isObject()) {          // 解析失败
        emit searchError("JSON解析失败");
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();            // 从文件中获得对象
    int status = root["status"].toInt();        // 检查状态码

    if (status != 1) {                  // 状态码不对，表示请求失败
        QString errorMsg = root["error_msg"].toString();
        emit searchError(errorMsg.isEmpty() ? "API返回错误" : errorMsg);
        reply->deleteLater();
        return;
    }

    try {
        QList<SongInfo> songs = handleSearchResult(doc);
        int total = root["data"].toObject()["total"].toInt();
        emit searchFinished(songs);
    } catch (const std::exception &e) {
        emit searchError(QString("处理结果出错: %1").arg(e.what()));
    }

    reply->deleteLater();
}

QList<SongInfo> KuGouSearch::handleSearchResult(const QJsonDocument &doc)
{
    QList<SongInfo> result;
    QJsonObject root = doc.object();
    QJsonObject data = root["data"].toObject();
    QJsonArray songs = data["info"].toArray();

    for (const QJsonValue &value : songs) {         // &表示引用，避免拷贝
        QJsonObject item = value.toObject();
        
        SongInfo info;
        info.name = item["songname"].toString();
        info.singer = item["singername"].toString("未知艺术家");
        info.album = item["album_name"].toString("未知专辑");
        info.duration = item["duration"].toInt();
        info.interval = formatTime(info.duration);
        info.source = "KuGou";
        
        result.append(info);
    }

    return result;
}

QString KuGouSearch::formatTime(int seconds)
{
    int mins = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(mins).arg(secs, 2, 10, QChar('0'));
}