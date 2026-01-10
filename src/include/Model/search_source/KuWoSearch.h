#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ISearchSource.h"

class KuWoSearch : public ISearchSource
{
    Q_OBJECT

public:
    explicit KuWoSearch(QObject *parent = nullptr);
    void searchMusic(const QString songName) override;
    void searchMusic(const QString keyword, int page, int limit) override;
    QString sourceName() const override { return "酷我音乐"; }
    QString sourceId() const override { return "kw"; }

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QString formatTime(int seconds);
    QList<SongInfo> handleSearchResult(const QJsonArray &doc);
    
    QString encodeURIComponent(const QString& str) {
        // 仅保留安全字符（-_.!~*'()），其他字符转 %XX 编码
        return QUrl::toPercentEncoding(str, "-_.!~*'()");
    }
};