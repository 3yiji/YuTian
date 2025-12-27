#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QObject>

#include "KuGouSearch.h"
#include "SearchSong.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SearchSong searchSong;

    QObject::connect(&searchSong, &SearchSong::songListReady,
            [&](const int status, const QHash<QString, QList<SongInfo>> &sourceSongList){
        qDebug() << "SearchSong got status =" << status;
    });

    searchSong.getSongList("成都");
    return app.exec();
}