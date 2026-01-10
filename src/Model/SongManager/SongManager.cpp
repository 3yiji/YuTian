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
// #include <QPromise>
#include <QFuture>

SongManager::SongManager(NodeProcessManager* nodeManager, QObject *parent)
    : QObject(parent)
{
    nodeProcessManager = nodeManager;
    sourceControl = new SourceControl(nodeProcessManager, this);
    musicUrl = new MusicUrl(nodeProcessManager, sourceControl, this);

    searchSources.insert("kw", new KuWoSearch(this));
    searchSources.insert("kg", new KuGouSearch(this));
    searchSources.insert("tx", new QQSearch(this));
    searchSources.insert("wy", new NetEaseSearch(this));
    searchSources.insert("mg", new MiGuSearch(this));
}



void SongManager::initSource(QUrl scriptPath){
    
}


QFuture<QList<SongInfo>> SongManager::getSongInfoList(QString source, QString keyword, int page, int pageSize){
    // QPromise<QList<SongInfo>> promise;
    // ISearchSource* searchSource = searchSources.value(source, nullptr);
    // if((!searchSource) || (searchSource->sourceId() != source)){            // 没有相关的源，返回失败
    //     emit getSongInfoListFinished(false, QList<SongInfo>());
    //     promise.setException(std::make_exception_ptr(std::runtime_error("Source not found")));
    //     promise.finish();
    //     return promise.future();
    // }
    // connect(searchSource, &ISearchSource::searchFinished, this, [this, source, promise2 = std::move(promise)](const QList<SongInfo> &songList) mutable {
    //     QList<SongInfo> modifiedList = songList;  // 复制（必要）
    //     for(SongInfo &song : modifiedList){         // 修改每个歌曲的信息，添加来源
    //         song.source = source;
    //     }
    //     emit getSongInfoListFinished(true, modifiedList);

    //     promise2.addResult(modifiedList);
    //     promise2.finish();
    // }, Qt::SingleShotConnection);
    
    // searchSource->searchMusic(keyword, page, pageSize);            // 触发动作
    // return promise.future();

    // auto promise = std::make_shared<QPromise<QList<SongInfo>>>();
    // promise->start();
    return sourceControl->getSourceInfoMap()
    .then([this, source, keyword, page, pageSize](QMap<QString,SourceControl::SourceInfo> const &infoMap){
        ISearchSource* searchSource = searchSources.value(source, nullptr);
        if((!searchSource) || (searchSource->sourceId() != source)){            // 没有相关的源，返回失败
            qDebug() << "Source not found:" << source;
            emit getSongInfoListFinished(false, QList<SongInfo>());
            throw std::runtime_error("Source not found");
        }

        auto promise = std::make_shared<QPromise<QList<SongInfo>>>();           // 重新用promise包装一下
        connect(searchSource, &ISearchSource::searchFinished, this, [this, source, promise](const QList<SongInfo> &songList) mutable {
            qDebug() << "Search finished for source:" << source << "with" << songList.size() << "results.";
            QList<SongInfo> modifiedList = songList;  // 复制（必要）
            for(SongInfo &song : modifiedList){         // 修改每个歌曲的信息，添加来源
                song.source = source;
            }
            emit getSongInfoListFinished(true, modifiedList);
            promise->addResult(modifiedList);
            promise->finish();
        }, Qt::SingleShotConnection);
        connect(searchSource, &ISearchSource::searchError, this, [this, source, promise](const QString &errorMsg) mutable {
            emit getSongInfoListFinished(false, QList<SongInfo>());
            promise->setException(std::make_exception_ptr(std::runtime_error(errorMsg.toStdString())));
            promise->finish();
        }, Qt::SingleShotConnection);
        searchSource->searchMusic(keyword, page, pageSize);            // 触发动作
        return promise->future();
    }).unwrap();
}

QFuture<SongInfo> SongManager::getSongFileInfo(QString source, SongInfo songInfo){
    auto promise = std::make_shared<QPromise<SongInfo>>();
    promise->start();
    sourceControl->getSourceInfoMap()
    .then([this, source, songInfo](QMap<QString,SourceControl::SourceInfo> const &infoMap){  // 获取所有源信息
        QList<QPair<QString,SourceControl::SourceInfo>> sourceInfoList;
        for(auto it = infoMap.begin(); it != infoMap.end(); ++it){
            if(it.key() != source){
                sourceInfoList.append(qMakePair(it.key(), it.value()));
            }
            else{
                sourceInfoList.insert(0, qMakePair(it.key(), it.value()));
            }
        }

        return getSongFileInfoNext(0, sourceInfoList, songInfo);                    // 递归获取歌曲信息和下载
    })
    .unwrap()
    .then([promise](SongInfo songInfo){
        promise->addResult(songInfo);
        promise->finish();
    })
    .onFailed([promise](const std::exception &e){
        promise->setException(std::make_exception_ptr(e));
        promise->finish();
    });
    return promise->future();
}

QFuture<SongInfo> SongManager::getSongFileInfoNext(int index, QList<QPair<QString,SourceControl::SourceInfo>> sourceInfoList, SongInfo songInfo){
    // 终止条件
    if(index >= sourceInfoList.size()){
        QPromise<SongInfo> promise;
        promise.start();
        promise.setException(std::make_exception_ptr(std::runtime_error("All sources failed")));
        promise.finish();
        return promise.future();
    }

    return getSongInfoList(sourceInfoList.at(index).first, songInfo.name, 1, 10)
    // 第一步：获取歌曲信息
    .then([this, songInfo](QList<SongInfo> songList) -> SongInfo {
        if(songList.isEmpty()){
            throw std::runtime_error("failed to get song info");
        }

        for(const SongInfo &info : songList){
            if(info.name == songInfo.name && info.singer == songInfo.singer){
                return info;
            }
        }

        throw std::runtime_error("song not found in source");
    })
    // 第二步：下载歌曲
    .then([this, index, sourceInfoList](SongInfo songInfo) {
        auto promise = std::make_shared<QPromise<SongInfo>>();
        promise->start();
        
        connect(musicUrl, &MusicUrl::downloadSongFinished, this,
            [promise](bool success, QFileInfo fileInfo, SongInfo songInfo){
            if(success) {
                songInfo.filePath = fileInfo.filePath();
                promise->addResult(songInfo);
            } else {
                promise->setException(std::make_exception_ptr(std::runtime_error("Download failed")));
            }
            promise->finish();
        }, Qt::SingleShotConnection);
        songInfo.quality = sourceInfoList.at(index).second.qualitys.first();
        musicUrl->downloadSong(songInfo.source, songInfo);
        return promise->future();
    }).unwrap()
    .then([](SongInfo songInfo){
        auto promise = std::make_shared<QPromise<SongInfo>>();
        promise->start();
        promise->addResult(songInfo);
        promise->finish();
        return promise->future();
    })
    // 失败时尝试下一个源
    .onFailed([this, index, sourceInfoList, songInfo](const std::exception &e) {
        qDebug() << "Source" << index << "failed:" << e.what() << ", trying next...";
        return getSongFileInfoNext(index + 1, sourceInfoList, songInfo);
    })
    .unwrap();
}



