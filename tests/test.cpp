// #include "KuGouSearch.h"

// int main(int argc, char *argv[])
// {
//     KuGouSearch search;
//     search.searchMusic("成都", 1, 20);

//     return -1;
// }


#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include "Model/search_source/KuGouSearch.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    KuGouSearch search(nullptr); // 或者 KuGouSearch search; 都可以

    QObject::connect(&search, &KuGouSearch::searchFinished,
                     [&](int total, const QList<SongInfo>& songs) {
        qDebug() << "OK total =" << total << "got =" << songs.size();
        QCoreApplication::quit();
    });

    QObject::connect(&search, &KuGouSearch::searchError,
                     [&](const QString& err) {
        qDebug() << "ERR:2" << err;
        QCoreApplication::quit();
    });

    // 超时退出，防止卡住
    // QTimer::singleShot(10000, &app, []{
    //     qDebug() << "TIMEOUT";
    //     QCoreApplication::quit();
    // });

    search.searchMusic(QStringLiteral("成都"), 1, 20);
    return app.exec();
}