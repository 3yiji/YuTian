#pragma once
#include <QObject>
#include "NodeProcessManager.h"
#include "YuTian.h"
#include <QHash>
#include "SourceControl.h"
#include <QThread>
#include <QNetworkAccessManager>
#include <QDir>


class MusicUrl : public QObject
{
    Q_OBJECT;
public:
    explicit MusicUrl(NodeProcessManager* nodeManager, SourceControl* sourceControl, QObject *parent = nullptr);
    void downloadSong(QString source, SongInfo songInfo);

    void test(){
        SongInfo info;
        info.name = "成都";
        info.singer = "赵雷";
        info.songmid = "1106531626";
        info.id = "1106531626";
        info.quality = "128k";

        connect(this, &MusicUrl::downloadSongFinished, this,
                [](bool success, QFileInfo fileInfo, SongInfo songInfo){
            if(success){
                qDebug() << "Music URL fetched successfully:" << songInfo.musicUrl;
            } else {
                qDebug() << "Failed to fetch Music URL.";
            }
        });

        // QThread::sleep(3); // 等待1秒，确保测试环境稳定
        this->downloadSong("mg", info);
    }
signals:
    void getMusicUrlFinished(bool success, SongInfo songinfo);
    void downloadSongFinished(bool success, QFileInfo fileInfo, SongInfo songInfo);

private slots:
    void handleMatchedResponse(QString requestId, QJsonObject responseBody, bool success, QString errorMsg);


private:
    NodeProcessManager* nodeProcessManager;
    int m_requestIdCounter = 0;

    QHash<QString, SongInfo> pendingRequests;
    SourceControl* sourceControl;
    QNetworkAccessManager* networkManager;
    void getMusicUrl(QString source, SongInfo songinfo);
    

    QDir songDownloadDir = QDir(QDir::homePath() + "/YuTianDownloads");
};