#include "MusicUrl.h"
#include <QNetworkReply>
#include <QFile>

MusicUrl::MusicUrl(NodeProcessManager* nodeManager, SourceControl* sourceControl, QObject *parent)
    : QObject(parent)
{
    nodeProcessManager = nodeManager;
    this->sourceControl = sourceControl;

    networkManager = new QNetworkAccessManager(this);

    connect(nodeProcessManager, &NodeProcessManager::responseReceived, this, &MusicUrl::handleMatchedResponse);
}

void MusicUrl::getMusicUrl(QString source, SongInfo songinfo){
    if(!pendingRequests.isEmpty()){                     // 有未完成请求，拒绝新请求
        emit getMusicUrlFinished(false, SongInfo());
        return;
    }
    // 准备发送给 Node.js 进程的请求数据
    QJsonObject requestParams = QJsonObject{
        {"source", source},
        {"action", "musicUrl"},
        {"info", QJsonObject{
            {"type", "128k"},
            {"musicInfo", QJsonObject{
                {"name", songinfo.name},
                {"singer", songinfo.singer},
                {"album", songinfo.album},
                {"interval", songinfo.interval},
                {"duration", songinfo.duration},
                {"songmid", songinfo.songmid},
                {"quality", songinfo.quality}
            }}
        }}
    };
    

    int timeout = 5000; // 5秒超时
    QString requestId = "getMusicUrl_" + QString::number(++m_requestIdCounter);
    pendingRequests.insert(requestId, songinfo);

    QJsonObject requestBody = QJsonObject{
        {"method", "request"},
        {"params", requestParams}
    };

    nodeProcessManager->request(requestId, requestBody, timeout);
}

void MusicUrl::handleMatchedResponse(QString requestId, QJsonObject responseBody, bool success, QString errorMsg){
    
    
    if(!pendingRequests.contains(requestId)) return; // 非匹配请求，忽略

    if (!success) {
        qWarning() << "NodeProcessManager response error:" << errorMsg;
        emit getMusicUrlFinished(false, SongInfo());
        return;
    }
    // 处理返回的数据
    if (responseBody.value("success").toBool() == false) {
        emit getMusicUrlFinished(false, SongInfo());
        return;
    }

    // 验证url是否可用
    QString musicUrl = responseBody["data"].toString();
    QUrl url(musicUrl);
    if (!url.isValid()) {
        emit getMusicUrlFinished(false, SongInfo());
        return;
    }

    SongInfo songinfo = pendingRequests.value(requestId);
    songinfo.musicUrl = musicUrl;
    emit getMusicUrlFinished(true, songinfo);
    pendingRequests.remove(requestId);
}

void MusicUrl::downloadSong(QString source, SongInfo songInfo){ 
    QString savePath = songDownloadDir.filePath(songInfo.name + "-" + songInfo.singer + ".mp3");
    if(QFile::exists(savePath)){          // 文件已存在，直接返回成功
        emit downloadSongFinished(true, QFileInfo(savePath), songInfo);
        return;
    }

    getMusicUrl(source, songInfo);
    connect(this, &MusicUrl::getMusicUrlFinished, this, [this](bool success, SongInfo info){
        if(!success){               // 失败获取音乐链接，直接返回
            emit downloadSongFinished(false, QFileInfo(), info);
            return;
        }

        QUrl url(info.musicUrl);
        if (!url.isValid()) {                   // URL无效，返回失败
            emit downloadSongFinished(false, QFileInfo(), info);
            return;
        }

        // 创建网络请求
        QNetworkRequest request(url);
        // 可选：添加请求头（解决部分服务器拒绝下载的问题）
        // request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");

        // 发送GET请求，接收回复
        QNetworkReply *reply = networkManager->get(request);

        // 下载完成处理
        connect(reply, &QNetworkReply::finished, this, [=]() {
            if (reply->error() != QNetworkReply::NoError) {
                emit downloadSongFinished(false, QFileInfo(), info);
                reply->deleteLater();
                return;
            }

            // 保存文件到本地
            QString savePath = songDownloadDir.filePath(info.name + "-" + info.singer + ".mp3");
            // 确保下载目录存在
            if (!songDownloadDir.mkpath(".")) {
                qDebug() << "创建下载目录失败：" << songDownloadDir.absolutePath();
                reply->deleteLater();
                emit downloadSongFinished(false, QFileInfo(), info);
                return;
            }
            QFile file(savePath);
            if (!file.open(QIODevice::WriteOnly)) {
                qDebug() << "文件打开失败：" << savePath;
                reply->deleteLater();
                emit downloadSongFinished(false, QFileInfo(), info);
                return;
            }

            // 写入下载的音频数据
            file.write(reply->readAll());
            file.close();
            qDebug() << "下载完成，文件保存至：" << savePath;
            emit downloadSongFinished(true, QFileInfo(savePath), info);

            reply->deleteLater(); // 释放reply资源
        }, Qt::SingleShotConnection);

    }, Qt::SingleShotConnection);


    
}