#include "SearchPage.h"
#include <QDebug>
#include "EventBus.h"

SearchPage::SearchPage(QObject *parent) : QObject(parent){
    searchSong = new SearchSong(this);                          // 搜索歌曲的类
    connect(EventBus::instance(), &EventBus::searchRequested,
            this, &SearchPage::onSearchRequested);              // 连接搜索框发来的信号
}

void SearchPage::performSearch(const QString &keyword)          // 搜索歌曲的函数
{
    // 在这里实现搜索逻辑，例如调用搜索源进行搜索
    qDebug() << "Performing search for keyword:" << keyword;
    // 你可以在这里实例化搜索源并调用它们的搜索方法
    setIsSearching(true);     // 正在进行搜索
    connect(searchSong, &SearchSong::songListReady,
            this, [this](int status, const QHash<QString, QList<SongInfo>> &sourceSongList){
        setIsSearching(false);            // 搜索完成
        this->sourceSongList = sourceSongList;
        setSrcIndex(1);           // 切换到第二个搜索源，触发歌曲的更新  
    }, Qt::SingleShotConnection);  // 只连接一次，避免重复触发
    searchSong->getSongList(keyword);
}

void SearchPage::updateSongList()    // 更新 m_songList 的内容
{
                  
    QString source = sourceList[m_srcIndex]; // 获取当前选择的搜索源名称
    if (!sourceSongList.contains(source)) {
        qDebug() << "No songs found for source:" << source;
        return;
    }

    const QList<SongInfo> &originSongList = sourceSongList.value(source);   // 引用原始的歌曲列表
    setTotalPages((originSongList.size() + pageSize - 1) / pageSize); // 计算总页数
    int startIndex = qMax(0, (m_pageIndex - 1) * pageSize);
    int endIndex = qMin(originSongList.size(), startIndex + pageSize);
    QVariantList newSongList; // 用于存储当前页的歌曲信息
    for (int i = startIndex; i < endIndex; ++i) {
        const SongInfo &song = originSongList[i];
        QVariantMap songMap;
        songMap["index"] = i + 1;
        songMap["name"] = song.name;
        songMap["singer"] = song.singer;
        songMap["album"] = song.album;
        songMap["duration"] = song.duration;
        newSongList.append(songMap);
    }
    setSongList(newSongList);

    qDebug() << "已经更新歌曲列表，当前页码：" << m_pageIndex << "歌曲数量：" << newSongList.size();
    // qDebug() << "songlist" << getSongList();
    // emit songListChanged();  // 通知界面更新
}