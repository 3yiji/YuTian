#pragma once
#include <QObject>
#include <QHash>
#include <QList>
#include <QUrl>
#include <QFile>
#include "ISearchSource.h"
#include <QHash>
#include "NodeProcessManager.h"
#include "SourceControl.h"
#include "MusicUrl.h"
#include <QFuture>
#include <QThread>


class SongManager : public QObject
{
    Q_OBJECT;
public:
    explicit SongManager(NodeProcessManager* nodeManager, QObject *parent = nullptr);
    void initSource(QUrl scriptPath);
    QFuture<QList<SongInfo>> getSongInfoList(QString source, QString keyword, int page, int pageSize);
    QFuture<SongInfo> getSongFileInfo(QString source, SongInfo songInfo);

void test(){
        SongInfo info;
        info.name = "成都";
        info.singer = "赵雷";
        info.songmid = "1106531626";
        info.id = "1106531626";
        info.quality = "128k";


        // QThread::sleep(10); // wait for 2 seconds to ensure initialization is complete
        this->getSongFileInfo("wy", info)
        .then([](SongInfo songInfo){
            qDebug() << "Downloaded song file path:" << songInfo.filePath;
        }).onFailed([](const std::exception &e){
            qDebug() << "Failed to download song:" << e.what();
        });

       
    }

signals:
    void initSourceFinished(bool success);
    void getSongInfoListFinished(bool success, QList<SongInfo> songList);
    void getMusicUrlFinished(bool success, QString musicUrl);

private slots:
    

private:

    bool initSourceResult = false;
    QHash<int, QVariant> pendingRequests;
    NodeProcessManager* nodeProcessManager;
    QHash<QString, ISearchSource*> searchSources;
    SourceControl* sourceControl;
    MusicUrl* musicUrl;
    QFuture<SongInfo> getSongFileInfoNext(int index, QList<QPair<QString,SourceControl::SourceInfo>> sourceInfoList, SongInfo songInfo);

};
