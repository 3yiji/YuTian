#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QObject>

#include "KuWoSearch.h"
#include "KuGouSearch.h"
#include "QQSearch.h"
#include "SearchSong.h"
#include "NetEaseSearch.h"
#include "MiGuSearch.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // SearchSong searchSong;

    // QObject::connect(&searchSong, &SearchSong::songListReady,
    //         [&](const int status, const QHash<QString, QList<SongInfo>> &sourceSongList){
    //     qDebug() << "SearchSong got status =" << status;
    // });

//  searchSong.getSongList("成都");

    MiGuSearch search;
    QObject::connect(&search, &MiGuSearch::searchFinished,
            [&](const QList<SongInfo> &songList){
        qDebug() << "MiGuSearch got" << songList.size() << "songs";
        for (const SongInfo &song : songList) {
            qDebug() << song.name << "-" << song.singer << "(" << song.album << ") [" << song.interval << "]";
        }
        app.quit();
    });
    QObject::connect(&search, &MiGuSearch::searchError,
            [&](const QString &error){
        qDebug() << "MiGuSearch error:" << error;
        app.quit();
    });

    search.searchMusic("chengdu");

    return app.exec();
}