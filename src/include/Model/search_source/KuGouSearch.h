#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct SongInfo {
    QString name;
    QString singer;
    QString album;
    QString interval;
    int duration;
    QString source;
};

class KuGouSearch : public QObject
{
    Q_OBJECT

public:
    explicit KuGouSearch(QObject *parent = nullptr);
    void searchMusic(const QString &keyword, int page = 1, int limit = 20);

signals:
    void searchFinished(int total, QList<SongInfo> songs);
    void searchError(const QString &errorMsg);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QString formatTime(int seconds);
    QList<SongInfo> handleSearchResult(const QJsonDocument &doc);
};
