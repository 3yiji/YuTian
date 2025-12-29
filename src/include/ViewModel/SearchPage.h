#pragma once
#include <QObject>
#include "SearchSong.h"
#include <QVariantList>
#include <qdebug.h>

class SearchPage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int srcIndex READ getSrcIndex WRITE setSrcIndex NOTIFY srcIndexChanged);
    Q_PROPERTY(int tabIndex READ getTabIndex WRITE setTabIndex NOTIFY tabIndexChanged);
    Q_PROPERTY(int pageIndex READ getPageIndex WRITE setPageIndex NOTIFY pageIndexChanged);
    Q_PROPERTY(int totalPages READ getTotalPages NOTIFY totalPagesChanged);
    Q_PROPERTY(QVariantList songList READ getSongList WRITE setSongList NOTIFY songListChanged);
    Q_PROPERTY(bool isSearching READ getIsSearching NOTIFY isSearchingChanged);

public:
    int getSrcIndex() const { return m_srcIndex; }
    void setSrcIndex(int index) {
        qDebug() << "setSrcIndex called with index:" << index;
        if (m_srcIndex != index){
            m_srcIndex = index;
            emit srcIndexChanged();
        } 
        setPageIndex(1); // 切换搜索源时，重置页码为1
        
    }

    int getTabIndex() const { return m_tabIndex; }
    void setTabIndex(int index) {
        if (m_tabIndex == index) return;
        m_tabIndex = index;
        emit tabIndexChanged();
    }

    int getPageIndex() const { return m_pageIndex; }
    void setPageIndex(int index) {
        if (m_pageIndex != index){
            m_pageIndex = index;
            emit pageIndexChanged();
        }
        updateSongList(); // 每次设置页码时，更新歌曲列表
        // qDebug() << "songList updated in setPageIndex:" << m_songList;
        
    }

    int getTotalPages() const { return m_totalPages; }
    void setTotalPages(int total) {
        if (m_totalPages == total) return;
        m_totalPages = total;
        emit totalPagesChanged();
    }

    QVariantList getSongList() const { return m_songList; }
    void setSongList(const QVariantList &list) {
        if(m_songList == list) return;
        m_songList = list;
        emit songListChanged();
    }

    bool getIsSearching() const { return m_isSearching; }
    void setIsSearching(bool searching) {
        if (m_isSearching == searching) return;
        m_isSearching = searching;
        emit isSearchingChanged();
        
    }


    explicit SearchPage(QObject *parent = nullptr);

    Q_INVOKABLE void performSearch(const QString &keyword);

    QList<QString> sourceList = {"KuWo", "KuGou", "QQ", "NetEase", "Migu"};

signals:
    void srcIndexChanged();
    void tabIndexChanged();
    void pageIndexChanged();
    void totalPagesChanged();
    void songListChanged();
    void isSearchingChanged();

private:
    int m_srcIndex = 0;
    int m_tabIndex = 0;
    int m_pageIndex = 1;
    int m_totalPages;
    QVariantList m_songList;
    bool m_isSearching = true;

    QHash<QString, QList<SongInfo>> sourceSongList;
    void updateSongList();    // 更新 m_songList 的内容

    SearchSong* searchSong;
    int pageSize = 10; // 每页显示的歌曲数量

private slots:
    void onSearchRequested(const QString &keyword) {
        performSearch(keyword);
    }

};
