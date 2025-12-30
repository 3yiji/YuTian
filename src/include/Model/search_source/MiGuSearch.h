#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ISearchSource.h"

class MiGuSearch : public ISearchSource
{
    Q_OBJECT
public:
    explicit MiGuSearch(QObject *parent = nullptr);
    void searchMusic(const QString songName) override;
    QString sourceName() const override { return "咪咕音乐"; }
    QString sourceId() const override { return "MiGu"; }

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    // 固定设备ID（从 JS 中提取）
    const QString FIXED_DEVICE_ID = "963B7AA0D21511ED807EE5846EC87D20";
    // 固定签名盐值（从 JS 中提取）
    const QString SIGNATURE_MD5 = "6cdc72a439cef99a3418d2a78aa28c73";
    const QString FIXED_SALT = "yyapp2d16148780a1dcc7408e06336b98cfd50";

    QNetworkAccessManager *m_manager;
    void searchMusic(const QString &keyword, int page, int limit);
    QList<SongInfo> handleSearchResult(const QJsonDocument &doc);
    QString formatTime(int seconds);

    struct SignData {
        QString deviceId; // 设备ID（固定值）
        QString sign;     // 生成的MD5签名
    };

    QString toMD5(const QString& data);
    SignData createSignature(const QString& time, const QString& str);
};