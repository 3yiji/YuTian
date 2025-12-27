#pragma once
#include <QObject>
#include "SearchSong.h"

class SearchPage : public QObject
{
    Q_OBJECT
    // Q_PROPERTY(SearchSong* searchSong READ getSearchSong CONSTANT)
public:
    explicit SearchPage(QObject *parent = nullptr);

    Q_INVOKABLE void performSearch(const QString &keyword);


private:
    SearchSong* searchSong;

};
