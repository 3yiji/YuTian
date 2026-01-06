#include "Model.h"
#include <NodeProcessManager.h>
#include <SongManager.h>
#include <QObject>

Model::Model(QObject *parent)
    : QObject(parent)
{
    nodeManager = new NodeProcessManager(this);
    songManager = new SongManager(nodeManager, this);
}

QList<SongInfo> Model::getSongInfoList(QString source, QString keyword, int page, int pageSize){
    QList<SongInfo> songList;
    connect(songManager, &SongManager::getSongInfoListFinished, this,
                [&](bool success, QList<SongInfo> songInfoList){
        emit getSongInfoListFinished(success, songInfoList);
    }, Qt::SingleShotConnection); // 单次连接
    songManager->getSongInfoList(source, keyword, page, pageSize);
    
    return songList;
}