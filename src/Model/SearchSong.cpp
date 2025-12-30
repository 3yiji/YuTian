#include "SearchSong.h"
#include "KuWoSearch.h"
#include "KuGouSearch.h"
#include "QQSearch.h"
#include "NetEaseSearch.h"
#include "MiGuSearch.h"

SearchSong::SearchSong(QObject *parent)
    : QObject(parent)
{
    searchSources.insert("KuWo", new KuWoSearch(this));
    searchSources.insert("KuGou", new KuGouSearch(this));
    searchSources.insert("QQ", new QQSearch(this));
    searchSources.insert("NetEase", new NetEaseSearch(this));
    searchSources.insert("MiGu", new MiGuSearch(this));
}

void SearchSong::getSongList(const QString name)
{
    
    if(songNameLast == name){                      // 如果搜索的歌名和上次相同，直接返回缓存结果
        emit songListReady(0, sourceSongList);
        return;
    }
    songNameLast = name;                 // 更新上次搜索的歌名

    numGettedSources = 0;
    sourceSongList.clear();                  // 清空之前的搜索结果
    for (const QString &source : sourceList) {
        ISearchSource* searchSource = searchSources.value(source, nullptr);
        if (!searchSource) continue;
        if (searchSource->sourceId() != source) continue;

        connect(searchSource, &ISearchSource::searchFinished, this, [this, source](const QList<SongInfo> &songList){
            numGettedSources++;             // 计数器加一
            sourceSongList[source] = songList;          // 存储该音乐源的搜索结果
            if(numGettedSources == searchSources.size()){ // 如果所有音乐源都已返回结果，发出信号
                emit songListReady(0, sourceSongList);      // 返回结果
            }
        }, Qt::SingleShotConnection);  // 使用 SingleShotConnection 避免重复连接
        connect(searchSource, &ISearchSource::searchError, this, [this, source](const QString &error){
            numGettedSources++;
            qWarning() << "SearchSong: " << source << "error:" << error;
            if(numGettedSources == searchSources.size()){ // 如果所有音乐源都已返回结果，发出信号
                emit songListReady(0, sourceSongList);      // 返回结果
            }
        }, Qt::SingleShotConnection);
        searchSource->searchMusic(name);  // ← 触发搜索
    }    
}
