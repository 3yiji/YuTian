#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
// #include <QAESEncryption> // 注意：Qt 原生无 AES-ECB，需引入轻量 AES 库
#include "qaesencryption.h"
#include "ISearchSource.h"

class NetEaseSearch : public ISearchSource
{
    Q_OBJECT
public:
    explicit NetEaseSearch(QObject *parent = nullptr);
    void searchMusic(const QString songName) override;
    QString sourceName() const override { return "网易云音乐"; }
    QString sourceId() const override { return "NetEase"; }

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    // EAPI 加密密钥（从 JS 中提取，固定值）
    const QString eapiKey = "e82ckenh8dichen8"; 
    // AES-128-ECB 填充模式：PKCS7（和 JS 一致）
    const int AES_PADDING = QAESEncryption::PKCS7;
    const QString USER_AGENT = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36";

    QString aesEncryptECB(const QByteArray& data, const QString& key);
    QUrlQuery eapi(const QString& url, const QVariantMap& data);

    QNetworkAccessManager *m_manager;
    void searchMusic(const QString &keyword, int page, int limit);
    QList<SongInfo> handleSearchResult(const QJsonDocument &doc);
    QString formatTime(int seconds);
    
};