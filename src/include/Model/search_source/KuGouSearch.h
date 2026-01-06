#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ISearchSource.h"



class KuGouSearch : public ISearchSource
{
    Q_OBJECT

public:
    explicit KuGouSearch(QObject *parent = nullptr);
    // 实现 ISearchSource 接口
    void searchMusic(const QString songName) override;
    void searchMusic(const QString keyword, int page, int limit) override;
    QString sourceName() const override { return "酷狗音乐"; }
    QString sourceId() const override { return "KuGou"; }
    
private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QString formatTime(int seconds);
    QList<SongInfo> handleSearchResult(const QJsonDocument &doc);
    // 原有的带分页参数的方法
    
};
