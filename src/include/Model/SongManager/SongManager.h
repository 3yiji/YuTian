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

class SongManager : public QObject
{
    Q_OBJECT;
public:
    explicit SongManager(NodeProcessManager* nodeManager, QObject *parent = nullptr);
    void initSource(QUrl scriptPath);
    void getSongInfoList(QString source, QString keyword, int page, int pageSize);
    void getSongFileInfo(QString source, SongInfo songInfo);

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


};
