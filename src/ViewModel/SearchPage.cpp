#include "SearchPage.h"
#include <QDebug>

SearchPage::SearchPage(QObject *parent) : QObject(parent){
    searchSong = new SearchSong(this);
}

void SearchPage::performSearch(const QString &keyword)
{
    // 在这里实现搜索逻辑，例如调用搜索源进行搜索
    qDebug() << "Performing search for keyword:" << keyword;
    // 你可以在这里实例化搜索源并调用它们的搜索方法
}