#pragma once
#include <QObject>
#include <QHash>
#include <QList>
#include <QUrl>
#include "ISearchSource.h"

class SongManager : public QObject
{
    Q_OBJECT;
public:
    explicit SongManager(QObject *parent = nullptr);
    QUrl getSongUrl(const SongInfo &songInfo);

private:
    struct SourceInfo{
        QString name;
        QString type;
        QList<QString> actions;
        QList<QString> qualitys;
    };

    QHash<QString, SourceInfo> sourcesInfo;

};
