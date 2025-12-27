#include "SearchSong.h"
#include "KuGouSearch.h"

SearchSong::SearchSong(QObject *parent)
    : QObject(parent)
{
    searchSources.insert("KuGou", new KuGouSearch(this));
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
        });
        connect(searchSource, &ISearchSource::searchError, this, [this, source](const QString &error){
            numGettedSources++;
            qWarning() << "SearchSong: " << source << "error:" << error;
            if(numGettedSources == searchSources.size()){ // 如果所有音乐源都已返回结果，发出信号
                emit songListReady(0, sourceSongList);      // 返回结果
            }
        });
        searchSource->searchMusic(name);  // ← 触发搜索
    }    
}
