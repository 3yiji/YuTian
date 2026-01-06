#pragma once
#include <QObject>
#include <NodeProcessManager.h>
#include <SongManager.h>
#include <QList>


class Model : public QObject
{
    Q_OBJECT;
public:
    
    explicit Model(QObject *parent = nullptr);


    QList<SongInfo> getSongInfoList(QString source, QString keyword, int page, int pageSize);

signals:
    void getSongInfoListFinished(bool success, QList<SongInfo> list);

private:
    NodeProcessManager* nodeManager;            // 接口进程管理器
    SongManager* songManager;                   // 歌曲管理器
};