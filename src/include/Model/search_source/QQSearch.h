#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ISearchSource.h"

class QQSearch : public ISearchSource
{
    Q_OBJECT
public:
    explicit QQSearch(QObject *parent = nullptr);
    void searchMusic(const QString songName) override;
    QString sourceName() const override { return "QQ"; }
    QString sourceId() const override { return "QQ"; }

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    void searchMusic(const QString &keyword, int page, int limit);
    QList<SongInfo> handleSearchResult(const QJsonDocument &doc);
    QString formatTime(int seconds);
};