// ISearchSource.h
#pragma once

#include <QObject>
#include "YuTian.h"


class ISearchSource : public QObject {
    Q_OBJECT
public:
    int sourceSelectedIndex = 0;
    int classSelectedIndex = 0;

    explicit ISearchSource(QObject *parent = nullptr) : QObject(parent) {}
    
    virtual void searchMusic(const QString songName) = 0;
    virtual void searchMusic(const QString songName, int page, int pageSize) = 0;
    virtual QString sourceName() const = 0;
    virtual QString sourceId() const = 0;
    
signals:
    void searchFinished(const QList<SongInfo> &songList);
    void searchError(const QString &error);
};