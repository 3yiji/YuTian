#pragma once

#include <QString>
#include <QVector>
#include <QHash>
#include <QObject>
#include "ISearchSource.h"

class SearchSong : public QObject{
    Q_OBJECT
public:
    explicit SearchSong(QObject *parent = nullptr);
    void getSongList(const QString name);

signals:
    void songListReady(const int status, const QHash<QString, QList<SongInfo>> &sourceSongList);

private:
    int numGettedSources;
    QHash<QString, ISearchSource*> searchSources;      // Qt 父对象销毁时，会自动 delete 所有子对象（通过 QObject 的析构函数）

    QString songNameLast;
    QList<QString> sourceList = {"KuWo", "KuGou", "QQ", "NetEase", "Migu"};
    
    QHash<QString, QList<SongInfo>> sourceSongList; 

    
};