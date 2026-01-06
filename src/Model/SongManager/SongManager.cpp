#include "SongManager.h"
#include <QUrl>
#include <QTextStream>
#include "GlobalObject.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QProcess>
#include <QObject>
#include "KuWoSearch.h"
#include "KuGouSearch.h"
#include "QQSearch.h"
#include "NetEaseSearch.h"
#include "MiGuSearch.h"

SongManager::SongManager(NodeProcessManager* nodeManager, QObject *parent)
    : QObject(parent)
{
    nodeProcessManager = nodeManager;
    sourceControl = new SourceControl(nodeProcessManager, this);
    musicUrl = new MusicUrl(nodeProcessManager, sourceControl, this);

    searchSources.insert("KuWo", new KuWoSearch(this));
    searchSources.insert("KuGou", new KuGouSearch(this));
    searchSources.insert("QQ", new QQSearch(this));
    searchSources.insert("NetEase", new NetEaseSearch(this));
    searchSources.insert("MiGu", new MiGuSearch(this));
}



void SongManager::initSource(QUrl scriptPath){
    
}


void SongManager::getSongInfoList(QString source, QString keyword, int page, int pageSize){
    ISearchSource* searchSource = searchSources.value(source, nullptr);
    if((!searchSource) || (searchSource->sourceId() != source)){            // 没有相关的源，返回失败
        emit getSongInfoListFinished(false, QList<SongInfo>());
    }
    connect(searchSource, &ISearchSource::searchFinished, this, [this, source](const QList<SongInfo> &songList){
        QList<SongInfo> modifiedList = songList;  // 复制（必要）
        for(SongInfo &song : modifiedList){         // 修改每个歌曲的信息，添加来源
            song.source = source;
        }
        emit getSongInfoListFinished(true, modifiedList);
    }, Qt::SingleShotConnection);
    
    searchSource->searchMusic(keyword, page, pageSize);            // 触发动作

}

void SongManager::getSongFileInfo(QString source, SongInfo songInfo){
    QHash<QString,SourceControl::SourceInfo> sourceInfoMap = sourceControl->getSourceInfoMap();

    musicUrl->downloadSong(source, songInfo);
    connect(musicUrl, &MusicUrl::downloadSongFinished, this,
            [this](bool success, QFileInfo fileInfo, SongInfo songInfo){
        if(success){
            emit getMusicUrlFinished(true, fileInfo.filePath());
        } else {
            emit getMusicUrlFinished(false, QString());
        }
    }, Qt::SingleShotConnection);


}
