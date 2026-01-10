#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>

#include "ISearchSource.h"

class NetEaseSearch : public ISearchSource
{
    Q_OBJECT
public:
    explicit NetEaseSearch(QObject *parent = nullptr);
    void searchMusic(const QString songName) override;
    void searchMusic(const QString keyword, int page, int limit) override;
    QString sourceName() const override { return "网易云音乐"; }
    QString sourceId() const override { return "wy"; }

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    const QString USER_AGENT = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36";

    QString aesEncryptECB(const QByteArray& data, const QString& key);
    QUrlQuery eapi(const QString& url, const QVariantMap& data);

    QNetworkAccessManager *m_manager;
    
    QList<SongInfo> handleSearchResult(const QJsonDocument &doc);
    QString formatTime(int seconds);
    
};